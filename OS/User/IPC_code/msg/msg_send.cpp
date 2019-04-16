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
		perror("Create message");
		exit(0);
	}
	my_msg message;
	int mytype = 1;
	char buf[MAX_TEXT];
	while(cin>>buf){
		message.msg_type = mytype;
		if(mytype == 2){
			mytype = 1;
		}else{
			mytype = 2;
		}
		strcpy(message.text, buf);
		int n = msgsnd(msg_id, (void*)&message, sizeof(buf), 0);
		if(n == -1){
			perror("Messsage send");
			exit(0);
		}
		cout<<"I'm Porcess 0 and I send message "<<buf<<endl;
	}
	if(msgctl(msg_id, IPC_RMID, 0) == -1){
		perror("Remove msg");
		exit(0);
	}
	return 0;
}
