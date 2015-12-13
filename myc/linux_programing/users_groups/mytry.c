/*************************************************************************
    > File Name: mytry.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月10日 星期四 15时35分29秒
 ************************************************************************/

#include<stdio.h>
#include<pwd.h>
#include<string.h>
#include<shadow.h>
int main(int argc, char *argv[])
{
	struct passwd *pwd;
	struct spwd *shadow;
	while((shadow = getspent()) != NULL)
	{
			printf("%-8s \n", shadow->sp_namp);
	}
	endspent();
}
