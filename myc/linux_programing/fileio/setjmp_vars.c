/*************************************************************************
    > File Name: setjmp_vars.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月09日 星期三 17时58分35秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<setjmp.h>

static jmp_buf env;

static void dojump(int nvar, int rvar, int vvar)
{
	printf("Inside dojump(): nvar = %d rvar = %d vvar = %d\n", nvar, rvar, vvar);
	longjmp(env, 1);
}

int main(int argc, char *argv[])
{
	int nvar;
	register int rvar;//the value is use often
	volatile int vvar;//when we use it, we need to check the value;
	nvar = 111;
	rvar = 222;
	vvar = 333;

	if(setjmp(env) == 0){
		nvar = 777;
		rvar = 888;
		vvar = 999;
		dojump(nvar,rvar, vvar);
	}
	else
	{
		printf("after long jump():nvar = %d rvar = %d vvar = %d\n",nvar, rvar, vvar);
	}

	exit(EXIT_SUCCESS);
}
