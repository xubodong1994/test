/*************************************************************************
    > File Name: tlpi_hdr.h
    > Author: 
    > Mail: 
    > Created Time: 2015年12月04日 星期五 10时43分09秒
 ************************************************************************/
#ifndef TLPI_HDR_H
#define TLPI_HDR_H

#include<sys/types.h>//type definiton
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>//system calls
#include<errno.h>
#include<string.h>

#include"get_num.h" //for handling numeric
#include"error_functions.h"//for errno 

typedef enum{FALSE,TRUE} Boolean;

#define min(m,n) ((m)<(n)?(m):(n))
#define max(m,n) ((m)>(n)?(m):(n))

#endif

