/*************************************************************************
    > File Name: open.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月06日 星期日 09时21分54秒
 ************************************************************************/
#include<sys/stat.h>
#include<fcntl.h>
#include"../lib/tlpi_hdr.h"
#include<stdio.h>
int main()
{
#define MAX_READ 20
	char buffer[MAX_READ] = {"242x\n342342342"};
	ssize_t numRead;
	printf("%s\n", buffer);

	numRead = read(STDIN_FILENO, buffer, MAX_READ);
	if(numRead == -1)
		errExit("read");
	buffer[numRead] = '\0';
	printf("The input data was: %s\n", buffer);
	printf("%ld\n", (long)numRead);
	exit(EXIT_SUCCESS);

}
