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
	int shm_id = shmget((key_t)1234, 0, 0);
	if(shm_id == -1){
		perror("Create shared memory");
		exit(0);
	}
	Company* s_company = (Company*)shmat(shm_id, NULL, 0);
	if(s_company == (void*)-1){
		perror("Mapping memory");
		exit(0);
	}
	while(true){
		if((*(s_company)).sig != 0){
			cout<<(*(s_company)).name<<" ";
			cout<<(*(s_company)).old<<" ";
			cout<<(*(s_company)).sig<<endl;
			(*s_company).sig = 0;
		}
	}
	if(shmdt(s_company) == -1){
		perror("Demap memory");
		exit(0);
	}
	return 0;
}
