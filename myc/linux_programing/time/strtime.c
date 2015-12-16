/*************************************************************************
    > File Name: strtime.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月15日 星期二 19时58分00秒
 ************************************************************************/

#define _XOPEN_SOURCE
#include<time.h>
#include<locale.h>
#include"tlpi_hdr.h"

#define SBUF_SIZE 1000

int main(int argc, char *argv[])
{
	struct tm tm;
	char sbuf[SBUF_SIZE];
	char *ofmt;

	if(argc < 3 || strcmp(argv[1], "--help") == 0)
	{
		usageErr("%s input-date-time informat [out-format]\n", argv[0]);
	}

	memset(&tm, 0, sizeof(struct tm));
	if(strptime(argv[1], argv[2], &tm) == NULL)
		fatal("strptime");

	tm.tm_isdst = -1;

	printf("calender time (seconds since Epoch): %ld\n", (long)mktime(&tm));
	ofmt = (argc > 3)?argv[3] : "%H:%M:%S %A, %d %B %Y %Z";
	if(strftime(sbuf, SBUF_SIZE, ofmt, &tm) == 0)
		fatal("strftime() yields:%s\n", sbuf);
	printf("strftime() yields:%s\n", sbuf);
	exit(EXIT_SUCCESS);
}

