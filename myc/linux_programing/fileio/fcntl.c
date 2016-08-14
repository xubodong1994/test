/*************************************************************************
    > File Name: fcntl.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月08日 星期二 17时54分25秒
 ************************************************************************/

#include<sys/stat.h>
#include<fcntl.h>
#include"tlpi_hdr.h"
//dup and dup2 by fcntl and close

int main(int argc, char *argv[])
{
	size_t len;
	ssize_t numWritten;
	int newfd;
	int fd;
	int flags;

	fd = open("11", O_CREAT|O_TRUNC|O_WRONLY, S_IRUSR|S_IWUSR);
	if(fd == -1)
		errExit("open");

	numWritten = write(fd, "try\n", 4);
	if(numWritten == -1)
		errExit("write");

	newfd = fcntl(fd, F_DUPFD, 0);
	if(newfd == -1)
		errExit("fcntl");

	numWritten = write(newfd, "dup\n", 4);
	if(numWritten == -1)
		errExit("write");

	if(close(fd) == -1)
		errExit("close");


	//dup2
	flags = fcntl(newfd, F_GETFL);
	if(flags == -1)
	{
		errno = EBADF;
		return -1;
	}

	flags |= O_APPEND;
	if(fcntl(newfd,F_SETFL, flags) == -1)
		errExit("fcntl");

	if(newfd == fd)
		return fd;

	flags = fcntl(fd, F_GETFL);
	if(flags != -1)
		if(close(fd) == -1)
			errExit("close");

	return fcntl(newfd, F_DUPFD, fd);

}
