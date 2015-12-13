/*************************************************************************
    > File Name: mem_segments.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月08日 星期二 20时27分25秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>

char globBuf[65536];//bbs
int primes[] = {2,3,5,7};//data

static int square(int x)// use a sign to represent the address(signtabel)
{
	int result;//text , allocat when running 
	result = x*x;//text
	return result;//register
}

static void doCalc(int val)//symbol table
{
	printf("The square of %d is %d\n", val, square(val));//symbol table and resure the address

	if(val < 1000){
		int t;
		t = val * val * val;
		printf("The cube of %d is %d\n", val, t);
	}
}

int main(int argc, char *argv[])
{
	static int key = 9973;
	static char mbuf[1024000];
	char *p;
	p = malloc(1024);
	doCalc(key);
	exit(EXIT_SUCCESS);
}
