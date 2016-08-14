/*************************************************************************
    > File Name: appand.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月08日 星期二 17时43分27秒
 ************************************************************************/

#include<sys/stat.h>
#include<fcntl.h>
#include"tlpi_hdr.h"

int main(int argc, char *argv)
{
	int fd;
	off_t offset;
	ssize_t numWrite;

	fd = open("11", O_WRONLY|O_CREAT|O_TRUNC|O_APPEND, S_IRUSR|S_IWUSR);
	if(fd == -1)
		errExit("open");
	
	numWrite = write(fd, "lllll", 5);
	if(numWrite == -1)
		errExit("write");

	offset = lseek(fd, 0, SEEK_SET);
	numWrite = write(fd, "lllll", 6);
	if(numWrite == -1)
		errExit("write");

}
