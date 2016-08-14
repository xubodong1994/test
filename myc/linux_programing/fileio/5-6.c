/*************************************************************************
    > File Name: 5-6.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月08日 星期二 18时30分45秒
 ************************************************************************/

#include<stdio.h>
#include<fcntl.h>
#include"tlpi_hdr.h"

int main()
{
	int fd1;
	int fd2;
	int fd3;
	int numWrite;
	off_t offset;

	fd1 = open("11", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
	if(fd1 == -1)
		errExit("open");
	fd2 = dup(fd1);
	if(fd2 == -1)
		errExit("dup");
	fd3 = open("11", O_RDWR);
	if(fd3 == -1)
		errExit("open");

	numWrite = write(fd1, "hello", 6);
	if(numWrite == -1)
		errExit("write");

	numWrite = write(fd2, "world", 6);
	if(numWrite == -1)
		errExit("write");
	
	offset = lseek(fd2, 0, SEEK_SET);
	if(offset == -1)
		errExit("lseek");

	numWrite = write(fd1, "HELLO", 6);
	if(numWrite == -1)
		errExit("write");

	numWrite = write(fd3, "GIDDAY", 6);
	if(numWrite == -1)
		errExit("write");

}
