/*************************************************************************
    > File Name: longjmp.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月09日 星期三 17时13分26秒
 ************************************************************************/

#include<setjmp.h>
#include"tlpi_hdr.h"

static jmp_buf env;//for setjmp to save the PC and top_stack
static void f2(void)
{
	setjmp(env);
//	longjmp(env, 2);
}

static void f1(int argc){
	if(argc == 1)
		longjmp(env,1);
	f2();
}

int main(int argc,char *argv[])
{
	printf("%d\n", argc);
	f2();
//	setjmp(env);
	longjmp(env,1);
/*	switch(setjmp(env))
	{
		case 0:
			printf("calling f1() after initial setjmp()\n");
			f1(argc);
			break;
		case 1:
			printf("we jumped back from f1\n");
			break;
		case 2:
			printf("we jumped back from f2\n");
			break;
	}*/

	exit(EXIT_SUCCESS);
}

