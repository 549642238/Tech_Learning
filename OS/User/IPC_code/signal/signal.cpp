#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
using namespace std;

void handle(int sig){
	if(sig == SIGUSR1){
		cout<<"Son: I receive sig."<<endl;
	}
}

int main(int argc, char** argv){
	int state;
	pid_t pid = fork();
	if(pid < 0){
		perror("Create process");
		exit(0);
	}else if(pid == 0){
		if(signal(SIGUSR1, handle) == SIG_ERR){
			perror("Bind signal handler");
			exit(0);
		}
		pause();
		exit(0);
	}else{
		cout<<"Father: I send signal"<<endl;
		sleep(1);
		kill(pid, SIGUSR1);
	}
	waitpid(pid, &state, 0);
	exit(0);
	return 0;
}
