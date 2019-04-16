#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
using namespace std;

int main(int argc, char** argv){
	if(mkfifo("myfifo",0777) < 0){
		perror("Make fifo");
	}
	int write_fd = open("myfifo", O_WRONLY);
	if(write_fd < 0){
		perror("Open fifo");
		exit(0);
	}
	int n;
	char buf[100];
	while(cin>>buf){
		if(write(write_fd, buf, 100) < 0){
			perror("Write fifo");
			break;
		}
	}
	cout<<"Close fifo"<<endl;
	close(write_fd);
	return 0;
}
