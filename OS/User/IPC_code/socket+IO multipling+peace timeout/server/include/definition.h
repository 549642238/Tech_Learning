/*
	Date: 2017.09.15 Fri. 11:07.
	Author: czh.
	Description: definition.h This head file define all constants and data structures that could be used in TinyLink application server.
	Input: None.
	Output: None.
*/
#ifndef _DEFINE_H
#define _DEFINE_H

#include <iostream>
#include <list>

const int MAX_ONLINE_CONNECTION = 300;						// 同时在线的最大连接数(服务器支持的同时在线人数上限)
const int MAX_HANDLED_EVENTS = 300;						// 调用一次epoll_wait能处理的最大事件数量(服务器支持的并发事件处理上限)
const int SERVERPORT = 8888;							// 服务器端口号
#define SERVERIP "10.214.149.119"						// 服务器IP地址
const int MAX_IO_BUFF = 4096;							// 服务器和客户端进行IO通信的缓存大小
const int MAX_INTERVAL = 5;							// 服务器和客户端最长通信间隔时间(s)
struct ClientFd{								// 与客户连接的套接字信息
	int client_fd;								// 套接字ID
	int interval;								// 客户套接字距离上次通信响应的时间间隔
};
struct Para{
	int currentConnection;
	std::list<ClientFd> connectionSet;
};

#endif
