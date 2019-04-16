/*
	完整的消费者生产者程序：
	semaphore mutex = 1;
	semaphore empty = N;
	semaphore full = 0;
	void provider(){
		while(true){
			item = produce();
			down(&empty);
			down(&mutex);
			insert(item);				// 临界区操作：往仓库添加item
			up(&mutex);
			up(&full);
		}
	}
	
	void consumer(){
		while(true){
			down(&full);
			down(&mutex);
			item = remove();			// 临界区操作：从仓库拿取item
			up(&mutex);
			up(&empty);
			consume(item);
		}
	}
	为什么需要两个信号量empty和full，换成1个sem如何？
	semaphore mutex = 1;
	semaphore sem = 0;
	void provider(){
		while(true){
			item = produce();
			up(&sem);
			down(&mutex);
			insert(item);				// 临界区操作：往仓库添加item
			up(&mutex);
		}
	}
	
	void consumer(){
		while(true){
			down(&sem);
			down(&mutex);
			item = remove();			// 临界区操作：从仓库拿取item
			up(&mutex);
			consume(item);
		}
	}
	假设sem为0，仓库没有item。执行至provider::up(&sem)后，sem = 1，但此时没有执行provider::down(&mutex)，仓库还没加入item，CPU调度执行
	consumer::down(&sem)，sem变成0，执行consume::down(&mutex),item=remove()，其实仓库没有item，出错。
*/

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

#define BUF 10

union semun{  
	int val;  
	struct semid_ds *buf;  
	unsigned short *arry;
};

int main(int argc, char** argv){
	int sem_id = semget((key_t)1234, 1, 0666|IPC_CREAT);
	if(sem_id == -1){
		perror("Create semaphore");
		exit(0);
	}
	union semun sem_union;
	sem_union.val = BUF;
	if(semctl(sem_id, 0, SETVAL, sem_union) == -1){
		perror("Set semaphore");
		exit(0);
	}
	while(true){
		struct sembuf sem_buf;
		sem_buf.sem_num = 0;
		sem_buf.sem_op = 1;
		sem_buf.sem_flg = SEM_UNDO;
		if(semop(sem_id, &sem_buf, 1) == -1){
			if(errno == EAGAIN){
				cout<<"I'm full ..."<<endl;
			}else{
				perror("Operate semaphore");
				break;
			}
		}else{
			cout<<"I provide 1 resource"<<endl;
			int currentVal;
			if((currentVal = semctl(sem_id, 0, GETVAL, sem_union)) == -1){
				perror("Get value");
				exit(0);
			}
			cout<<"Current resource = "<<currentVal<<endl;
		}
		sleep(1);
	}
	if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1){
		perror("Delete semaphore");
		exit(0);
	}
	return 0;
}
