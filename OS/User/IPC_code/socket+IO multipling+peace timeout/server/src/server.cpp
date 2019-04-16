/*
	Date: 2017.09.18 Mon. 13:37.
	Author: czh.
	Description: Response all kinds of requests from TinyLink application client.
	Input: None.
	Output: None.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cJSON.h"
#include "definition.h"
#include "server.h"

using namespace std;

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
*  Description: Check every connection between client and server.
*  Parameter: None.
*  Return: None.
*/
void* checkConnection(void* arg){
	Para* para = (Para*)arg;
	while(true){
		/*
			有缺陷的程序，这里实际上应该给整个循环上锁，因为循环中的每个连接可能会在遍历过程中（还未遍历到）被客户端主动关闭或异常发生，导致遍历时访问的迭代器失效，只有给整个循环上锁也即对集合connectionSet上锁才能避免。但给整个循环上锁又会带来效率的问题。上锁的代码不能和其他线程并行，使得服务器每次都要检查完所有连接才能继续执行，如果并发连接很多会导致服务器响应慢的问题。由此看来，一般服务器不会做socket连接长时间没有IO则关闭的检查，应该放在客户端，由客户端自己检查。
		*/
		for(list<ClientFd>::iterator it=(para->connectionSet).begin();it!=(para->connectionSet).end();){
			if((time(NULL) - it->interval) > MAX_INTERVAL){
				pthread_mutex_lock(&counter_mutex);
				close(it->client_fd);
				--(para->currentConnection);
				it = (para->connectionSet).erase(it);
				pthread_mutex_unlock(&counter_mutex);
				cout<<"Check connection: Existing client connetion has been closed. Current online people is "<<para->currentConnection<<"."<<endl;
			}else{
				++it;
			}
		}
		sleep(1);
	}
	return NULL;
}

/*
*  Description: Fetch current time.
*  Parameter: None.
*  Return: Current time&date in string format.
*/
string Server::getTime(){
	time_t t = time(NULL);
	tm* date = localtime(&t);
	char buf[30];
	strftime(buf, 30, "%Y-%m-%d %H:%M:%S %Z", date);
	string currentTime(buf);
	return currentTime;
}

/*
*  Description: Initialize server socket.
*  Parameter: None.
*  Return:>0    success, socket file descriptor
*         -1    Socket creation error
*         -2    Socket bind error
*	  -3	Socket listen error
*/ 
int Server::mk_socket(const int& port, const char* ip){
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	char buf[1024];
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, buf, 1024);
	if(server_fd == -1){
		perror("Socket creation error");
		return -1;
	}
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &(serveraddr.sin_addr));
	if(bind(server_fd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1){
		perror("Socket bind error");
		return -2;
	}
	if(listen(server_fd, 5) == -1){
		perror("Socket listen error");
		return -3;
	}
	return server_fd;
}

/*
*  Description: Add/Delete/Modify a (new/exist) event for a certain descriptor to list which will be checked by epoll.
*  Parameter: epoll fd, fd that will be added into/deleted from/modified in list, the state that epoll concerns.
*  Return: 0	Succcess
*	  -1	Add fail
*	  -2	Unknown operation
*/
int Server::operate_epoll_event(const int& epoll_fd, const int& new_fd, const int& state, const int& operation){
	struct epoll_event e_ev;
	e_ev.events = state;											// EPOLLIN for read, EPOLLOUT for write, EPOLLERR for error
	e_ev.data.fd = new_fd;
	int result = 0;
	switch(operation){
		case 0:{
			result = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &e_ev);				// 0 for success, -1 for error
			break;
		}
		case 1:{
			result = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, new_fd, &e_ev);
			break;
		}
		case 2:{
			result = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, new_fd, &e_ev);
			break;
		}
		default:{
			result = -2;
			break;
		}
	}
	return result;
}

/*
*  Description: Process connection requests from TinyLink application client.
*  Parameter: None.
*  Return: None.
*/
void Server::start(const int& port, const char* ip){
	cout<<"TinyLink application server start: "<<getTime()<<endl;
	cout<<"Server ip = "<<ip<<", port = "<<port<<endl;
	cout<<"Current connections: "<<para.currentConnection<<endl;
	int server_fd = mk_socket(port, ip);									// Server socket file descriptor
	if(server_fd < 0){
		cout<<"Serious error occurs in socket initialization, server will be closed."<<endl;
		exit(0);
	}
	struct epoll_event* events = new epoll_event[MAX_HANDLED_EVENTS];					// Events to be handled when epoll_wait returns
	int epoll_fd = epoll_create(MAX_ONLINE_CONNECTION);							// Epoll file descriptor
	if(epoll_fd == -1){
		perror("Initialize epoll fail");
		exit(0);
	}
	if(operate_epoll_event(epoll_fd, server_fd, EPOLLIN, 0) < 0){						// Add server socket(read event) into epoll checking list
		cout<<"Serious error occurs in epoll operation, server will be closed."<<endl;
		exit(0);
	}
	int result = 0;
	pthread_t id;
	int ret = pthread_create(&id, NULL, checkConnection, (void*)&para);
	if(ret < 0){
		perror("Create thread");
		exit(0);
	}
	while(true){
		result = epoll_wait(epoll_fd, events, MAX_HANDLED_EVENTS, -1);					// Epoll checks all interested fds, events will be filled in content when any fds are ready
		if(result <= 0){
			perror("Epoll wait error");
			continue;
		}
		handle_events(epoll_fd, events, result, server_fd);						// Process ready events by multi-thread
	}
	pthread_join(id, NULL);
	delete[] events;
}

/*
*  Description: Process epoll ready socket fd with corresponding events.
*  Parameter: epoll fd, ready events, number of events, server fd.
*  Return: None.
*/
void Server::handle_events(const int& epoll_fd, const struct epoll_event* events, const int& result, const int& server_fd){
	int fd;
	for(int i=0;i<result;i++){
		fd = events[i].data.fd;
		if(events[i].events & EPOLLIN){
			if(fd == server_fd){
				handle_accept(epoll_fd, server_fd);
			}else{
				handle_read(epoll_fd, fd);
			}
		}else if(events[i].events & EPOLLOUT){								// Here, write process is ignored because epoll io is context free, but we want response to client according to corresponding request
			handle_write(epoll_fd, fd);
		}
	}
}

/*
*  Description: Process accept event.
*  Parameter: epoll fd, server socket fd.
*  Return: None.
*/
void Server::handle_accept(const int& epoll_fd, const int& server_fd){
	if(para.currentConnection < MAX_ONLINE_CONNECTION){
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen;
		int client_fd = accept(server_fd, (struct sockaddr*)&clientaddr, &clientaddrlen);
		if(client_fd == -1){
			perror("Accept error");
		}else{
			ClientFd temp;
			temp.client_fd = client_fd;
			temp.interval = time(NULL);
			pthread_mutex_lock(&counter_mutex);
			para.connectionSet.push_back(temp);
			++para.currentConnection;
			pthread_mutex_unlock(&counter_mutex);
			if(operate_epoll_event(epoll_fd, client_fd, EPOLLIN, 0) < 0){
				cout<<"Fail to add client fd into epoll list."<<endl;
			}
			cout<<"New client connetion has been established. Current online people is "<<para.currentConnection<<"."<<endl;
		}
	}else{
		cout<<"Client cannot connect, pending ..."<<endl;
	}
}

/*
*  Description: Process read event(Request data comes from client).
*  Parameter: epoll fd, client socket fd.
*  Return: None.
*/
void Server::handle_read(const int& epoll_fd, const int& fd){
	int nread;
	char buf[MAX_IO_BUFF];
	nread = read(fd, buf, MAX_IO_BUFF);
	if(nread == -1){
		perror("Read error, the client will be closed");
		if(operate_epoll_event(epoll_fd, fd, EPOLLIN, 1) < 0){
			cout<<"Fail to delete client fd from epoll list."<<endl;
		}
		pthread_mutex_lock(&counter_mutex);
		close(fd);
		--para.currentConnection;
		list<ClientFd>::iterator it = findItByFd(fd);
		if(it != para.connectionSet.end()){
			para.connectionSet.erase(it);
		}
		pthread_mutex_unlock(&counter_mutex);
		cout<<"Read error: Existing client connetion has been closed. Current online people is "<<para.currentConnection<<"."<<endl;
	}else if(nread == 0){
		cout<<"Client offline"<<endl;
		if(operate_epoll_event(epoll_fd, fd, EPOLLIN, 1) < 0){
			cout<<"Fail to delete client fd from epoll list."<<endl;
		}
		pthread_mutex_lock(&counter_mutex);
		close(fd);
		--para.currentConnection;
		list<ClientFd>::iterator it = findItByFd(fd);
		if(it != para.connectionSet.end()){
			para.connectionSet.erase(it);
		}
		pthread_mutex_unlock(&counter_mutex);
		cout<<"Client close: Existing client connetion has been closed. Current online people is "<<para.currentConnection<<"."<<endl;
	}else{
		list<ClientFd>::iterator it = findItByFd(fd);
		it->interval = time(NULL);
		cout<<buf<<endl;
		write(fd, "ACK", 3);
		// 处理请求
		// ...
	}
}

/*
*  Description: Process write event(Reponse to client with corresponding content).
*  Parameter: epoll fd, client socket fd.
*  Return: None.
*/
void Server::handle_write(const int& epoll_fd, const int& fd){
	int nwrite;
	char buf[MAX_IO_BUFF];
	strncpy(buf, "This cannot be executed.", 20);
	nwrite = write(fd, buf, MAX_IO_BUFF);
	if(nwrite == -1){
		perror("Write error, the client will be closed");
		if(operate_epoll_event(epoll_fd, fd, EPOLLOUT, 1) < 0){
			cout<<"Fail to delete client fd from epoll list."<<endl;
		}
		pthread_mutex_lock(&counter_mutex);
		close(fd);
		--para.currentConnection;
		list<ClientFd>::iterator it = findItByFd(fd);
		if(it != para.connectionSet.end()){
			para.connectionSet.erase(it);
		}
		pthread_mutex_unlock(&counter_mutex);
		cout<<"Write: Existing client connetion has been closed. Current online people is "<<para.currentConnection<<"."<<endl;
	}else{
		cout<<"Write success."<<endl;
	}
}

/*
*  Description: Find client information by fd.
*  Parameter: client fd.
*  Return: client structure.
*/
list<ClientFd>::iterator Server::findItByFd(int fd){
	list<ClientFd>::iterator it;
	for(it=para.connectionSet.begin();it!=para.connectionSet.end();++it){
		if(it->client_fd == fd){
			return it;
		}
	}
	return it;
}
