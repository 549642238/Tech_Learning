#include <cstring>
#include <string>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <errno.h>
#include "define.h"
using namespace std;

#define SIZE 10000

int main(int argc, char** argv){
	int shm_id = shmget((key_t)1234, SIZE, IPC_CREAT | 0666);
	if(shm_id == -1){
		perror("Create shared memory");
		exit(0);
	}
	Company* s_company = (Company*)shmat(shm_id, NULL, 0);
	if(s_company == (void*)-1){
		perror("Mapping memory");
		if(shmctl(shm_id, IPC_RMID, NULL) == -1){
			perror("Delete share memory");
			exit(0);
		}
		exit(0);
	}
	char name[10];
	int old;
	strncpy((*(s_company)).name, "0000000", 10);
	(*(s_company)).old = 0;
	(*(s_company)).sig = 0;
	while(true){
		if((*(s_company)).sig == 0){
			cout<<"Input company information(name id):"<<endl;
			cin>>name>>old;
			if(strcmp(name, "end") == 0){
				break;
			}else{
				strncpy((*(s_company)).name, name, 10);
				(*(s_company)).old = old;
				(*(s_company)).sig = 1;
			}
		}
	}
	if(shmdt(s_company) == -1){
		perror("Demap memory");
		if(shmctl(shm_id, IPC_RMID, NULL) == -1){
			perror("Delete share memory");
			exit(0);
		}
		exit(0);
	}
	if(shmctl(shm_id, IPC_RMID, NULL) == -1){
		perror("Delete share memory");
		exit(0);
	}
	return 0;
}
