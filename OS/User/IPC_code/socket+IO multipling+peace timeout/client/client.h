#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

class Client{
private:
	int socketfd;
	const int MAXLINE;
	const int serverPort;
	const char* serverIP;
	Client(const Client& c):MAXLINE(1024), serverPort(0), serverIP(NULL){}
	Client operator=(const Client& c){
		return *this;
	}
public:
	Client(int port, const char* ip):MAXLINE(1024), socketfd(0), serverPort(port), serverIP(ip){
	}
	int run();
	~Client(){
	}
};

#endif
