#include "client.h"
using namespace std;
/*
int MAXALIVE = 5;
int alive = 0;
int socketfd = 0;

void handle_urg(int signo){
	char c;
	if(recv(socketfd, &c, 1, MSG_OOB) < 0){
		if(errno != EWOULDBLOCK){
			perror("Urg data");
			return;
		}
	}else{
		alive = 0;
	}
}

void handle_alrm(int signo){
	++alive;
	if(alive > MAXALIVE){
		close(socketfd);
		return;
	}
	send(socketfd, "1", 1, MSG_OOB);
	alarm(1);
}
*/
int Client::run(){
	//signal(SIGURG, handle_urg);
	//signal(SIGALRM, handle_alrm);
	struct sockaddr_in sockaddr;
	socketfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&sockaddr,0,sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(serverPort);
	inet_pton(AF_INET,serverIP,&sockaddr.sin_addr);
	if(connect(socketfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))<0){
		printf("connect error %s errno: %d\n",strerror(errno),errno);
		return -1;
	}
	cout<<"Connect success"<<endl;
	char buf[MAXLINE];
	//alarm(1);
	while(true){
		cout<<"Input content"<<endl;
		cin>>buf;
		if(write(socketfd, buf, MAXLINE)<0){
			perror("Write error");
			break;
		}else{
			int n = read(socketfd, buf, MAXLINE);
			if(n>0){
				cout<<buf<<endl;
			}else if(n==0){
				cout<<"Close socket"<<endl;
				break;
			}else{
				perror("Read error");
				break;
			}
		}
	}
	close(socketfd);
	return 0;
}
