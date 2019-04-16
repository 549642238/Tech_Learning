#include <cstring>
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
using namespace std;

int main(int argc, char** argv){
	FILE* fp = popen("ls -a", "r");
	if(fp == NULL){
		perror("Popen fail");
	}
	char buf[100];
	fread(buf, 100, 1, fp);
	cout<<buf<<endl;
	pclose(fp);
	return 0;
}
