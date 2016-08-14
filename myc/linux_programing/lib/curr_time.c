/*************************************************************************
    > File Name: curr_time.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月15日 星期二 19时50分01秒
 ************************************************************************/

#include<time.h>
#include"curr_time.h"

#define BUF_SIZE 1000

char *currTime(const char *format)
{
	static char buf[BUF_SIZE];
	time_t t;
	size_t s;
	struct tm *tm;
	t = time(NULL);
	tm = localtime(&t);
	if(tm == NULL)
		return NULL;

	s = strftime(buf, BUF_SIZE, (format != NULL) ? format:"%c", tm);
	return (s == 0)?NULL:buf;
}

