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

int main(int argc, char** argv){
	int fd[2];
	int state;
	pid_t pid;
	if(pipe(fd) < 0){
		perror("Pipe");
	}
	pid = fork();
	if(pid < 0){
		perror("Create process");
	}else if(pid == 0){
		close(fd[1]);
		char buf[12];
		int n = read(fd[0], buf, 12);
		cout<<buf<<endl;
		exit(0);
	}else{
		close(fd[0]);
		write(fd[1], "hello, son", 12);
	}
	waitpid(pid, &state, 0);
	return 0;
}
