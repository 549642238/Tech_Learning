#include <iostream>
#include <cstdio>
#include <pthread.h>
using namespace std;

const int N = 100;
int buffer = 0;

pthread_mutex_t mutex;
pthread_cond_t condc, condp;

void* consumer(void*){
	while(true){
		pthread_mutex_lock(&mutex);
		while(buffer == 0){
			/*
				当buffer为0，消费者无法从仓库拿取item，调用cond_wait阻塞等待condc信号，并释放mutex。
				等生产者++buffer后调用cond_signal通知持有condc信号的线程可以不必阻塞了，但wait从阻塞(就绪)到运行还需要对mutex上锁后继续执行
			*/
			pthread_cond_wait(&condc, &mutex);
		}
		--buffer;
		cout<<"Consume 1 resource, current buffer = "<<buffer<<endl;
		pthread_cond_signal(&condp);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

void* provider(void*){
	while(true){
		pthread_mutex_lock(&mutex);
		while(buffer == N){
			pthread_cond_wait(&condp, &mutex);
		}
		++buffer;
		cout<<"Produce 1 resource, current buffer = "<<buffer<<endl;
		pthread_cond_signal(&condc);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

int main(){
	pthread_t threadA, threadB;
	pthread_mutex_init(&mutex, 0);
	pthread_cond_init(&condc, 0);
	pthread_cond_init(&condp, 0);
	pthread_create(&threadA, 0, consumer, 0);
	pthread_create(&threadB, 0, provider, 0);
	pthread_join(threadA, 0);
	pthread_join(threadB, 0);
	pthread_cond_destroy(&condc);
	pthread_cond_destroy(&condp);
	pthread_mutex_destroy(&mutex);
	return 0;
}
