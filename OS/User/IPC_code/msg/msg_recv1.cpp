#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
using namespace std;

#define MAX_TEXT 512
struct my_msg{  
	long msg_type;  
	char text[MAX_TEXT];
}; 

int main(int argc, char** argv){
	int msg_id = msgget((key_t)1234, IPC_CREAT | 0666);
	if(msg_id == -1){
		perror("Open message");
		exit(0);
	}
	my_msg message;
	int mytype = 1;
	while(true){
		int n = msgrcv(msg_id, (void*)&message, MAX_TEXT, mytype, 0);
		if(n == -1){
			perror("Messsage receive");
			exit(0);
		}
		if(n == 0){
			cout<<"No message."<<endl;
			break;
		}
		cout<<"I'm Porcess 1 and I receive "<<message.text<<endl;
	}
	return 0;
}
