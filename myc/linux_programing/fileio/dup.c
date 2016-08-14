/*************************************************************************
    > File Name: dup.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月08日 星期二 18时16分36秒
 ************************************************************************/

#include<fcntl.h>
#include<unistd.h>
#include"tlpi_hdr.h"

extern char etext, edata, end;
int main(int argc, char *argv[])
{
	int newfd;
	int oldfd;
	ssize_t numWrite;
	int newflags, oldflags;
	newfd = open("11",O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
	if(newfd == -1)
		errExit("open");

	
	oldfd = dup(newfd);
	if(oldfd == -1)
		errExit("dup");

	numWrite = write(newfd, "1111\n", 5);
	if(numWrite == -1)
		errExit("write");

	numWrite = write(oldfd, "2222\n", 5);

	newflags = fcntl(newfd, F_GETFL);
	if(newflags == -1)
		errExit("fcntl");

	oldflags = fcntl(newfd, F_GETFL);
	if(oldflags == -1)
		errExit("fcntl");

	if(oldflags == newflags)
		printf("match\n");

	oldflags |= O_APPEND;
	if(fcntl(oldfd, F_SETFL, oldflags) == -1)
		errExit("fcntl");

	newflags = fcntl(newfd, F_GETFL);
	if(newflags == -1)
		errExit("fcntl");

	if(newflags == oldflags)
		printf("match\n");
	printf("%d %d %d\n", etext, edata, end);
}

