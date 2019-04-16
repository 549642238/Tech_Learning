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
	int read_fd = open("myfifo", O_RDONLY);
	if(read_fd < 0){
		perror("Open fifo");
		exit(0);
	}
	int n;
	char buf[100];
	while((n = read(read_fd, buf, 100)) > 0){
		cout<<"Read fifo: "<<buf<<endl;
	}
	cout<<"Close fifo"<<endl;
	close(read_fd);
	return 0;
}
