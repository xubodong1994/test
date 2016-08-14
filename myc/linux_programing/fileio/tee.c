/*************************************************************************
    > File Name: seek_io.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月06日 星期日 11时39分42秒
 ************************************************************************/

#include<stdio.h>
#include<fcntl.h>
#include<ctype.h>
#include"tlpi_hdr.h"

int main(int argc, char **argv)
{
	size_t len;
	off_t offset;
	int fd, ap, j;
	char *buf;
	char ch;
	ssize_t numRead, numWritten;
	if(argc < 2 || strcmp(argv[1], "--help") == 0)
		usageErr("tee [-a] file\n");


		switch(ch = getopt(argc, argv, ":a")){
			case -1:
				fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				if(fd == -1)
					errExit("open");
				break;
			case 'a':
				fd = open(argv[2], O_WRONLY | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
				if(fd == -1)
					errExit("open");
				break;
			default:
				break;
	}
		buf = malloc(1024);
		if(buf == NULL)
			errExit("malloc");

		numRead = read(STDIN_FILENO, buf, 1024);
		write(STDOUT_FILENO, buf, 1024);
		write(fd,buf,1024);
	exit(EXIT_SUCCESS);
}
