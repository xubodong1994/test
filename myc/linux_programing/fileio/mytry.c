/*************************************************************************
    > File Name: mytry.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月06日 星期日 16时10分15秒
 ************************************************************************/
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<errno.h>
#include"tlpi_hdr.h"
#define _GNU_SOURCE 

int main()
{
	int fd;
	
	printf("Generated filename was: %s\n%s\n", program_invocation_name, program_invocation_short_name);
}
