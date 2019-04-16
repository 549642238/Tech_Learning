#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <errno.h>
using namespace std;

int main(int argc, char** argv){
	int sem_id = semget((key_t)1234, 0, 0666);
	if(sem_id == -1){
		perror("Open semaphore");
		exit(0);
	}
	struct sembuf sem_buf;
	sem_buf.sem_num = 0;
	sem_buf.sem_op = -1;
	sem_buf.sem_flg = SEM_UNDO;
	while(true){
		if(semop(sem_id, &sem_buf, 1) == -1){
			if(errno == EAGAIN){
				cout<<"I'm waiting ..."<<endl;
			}else{
				perror("Operate semaphore");
				exit(0);
			}
		}else{
			cout<<"Consume 1 resource"<<endl;
		}
	}
	return 0;
}
