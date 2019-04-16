/*
	Date: 2017.09.18 Mon. 11:29.
	Author: czh.
	Description: server.h This head file define the behaviuors of TinyLink application server which is mainly responsible for handling data requests from TinyLink application client.
	Input: Configuration parameters for this server.
	Output: None.
*/
#ifndef _SERVER_H
#define _SERVER_H

#include <iostream>
#include <list>

class Server{
private:
	Para para;
protected:
	std::string getTime();
	int mk_socket(const int& port, const char* ip);
	int operate_epoll_event(const int& epoll_fd, const int& new_fd, const int& state, const int& operation);
	void handle_events(const int& epoll_fd, const struct epoll_event* events, const int& result, const int& server_fd);
	void handle_accept(const int& epoll_fd, const int& server_fd);
	void handle_read(const int& epoll_fd, const int& fd);
	void handle_write(const int& epoll_fd, const int& fd);
	std::list<ClientFd>::iterator findItByFd(int fd);
public:
	Server(){
		para.currentConnection = 0;
	}
	void start(const int& port, const char* ip);
	~Server(){
	}
};

#endif
