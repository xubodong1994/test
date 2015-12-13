/*************************************************************************
    > File Name: display_env.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月09日 星期三 11时41分44秒
 ************************************************************************/
#define _GNU_SOURCE
#include"tlpi_hdr.h"
#include<stdlib.h>

extern char **environ;

int main(int argc, char ** argv, char ** envp)
{
	int j;
	char **ep;
	clearenv();

	for(j = 1; j<argc; j++)
		if(putenv(argv[j]) != 0)
			errExit("putenv: %s", argv[j]);

	if(setenv("GREET", "hello world", 0) == -1)
		errExit("setenv");

	unsetenv("BYE");

	for(ep = environ; *ep != NULL; ep++)
		puts(*ep);

	exit(EXIT_SUCCESS);
}

