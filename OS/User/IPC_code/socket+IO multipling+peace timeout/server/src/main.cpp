/*
	Time:2016.09.18 Mon. 16:54.
	Author:czh.
	Description:Start server.
	Input:Command line arguments, the first argument is server port, the second argument is server ip. If no args are received, server will use default ip and port.
	Output:None.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "definition.h"
#include "server.h"

using namespace std;

int main(int argc,char* argv[]){
	Server server;
	if(argc > 2){
		server.start(atoi(argv[1]), argv[2]);
	}else{
		server.start(SERVERPORT, SERVERIP);
	}
	return 0;
}
