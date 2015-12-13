/*************************************************************************
    > File Name: check_passwd.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月10日 星期四 16时26分47秒
 ************************************************************************/
#define _BSD_SOURCE
#define _XOPEN_SOURCE

#include<crypt.h>
#include<unistd.h>
#include<stdio.h>
#include<limits.h>
#include<pwd.h>
#include<shadow.h>
#include"tlpi_hdr.h"

int main(int argc, char *argv[])
{
	char *username, *password, *encrypted, *p;
	struct passwd *pwd;
	struct spwd *spwd;
	Boolean authOK;

	size_t len;
	long lnmax;

	lnmax = sysconf(_SC_LOGIN_NAME_MAX);//test the value of the mecros;
	if(lnmax == -1)// -1 mean that there is no limits
		lnmax = 256;

	username = malloc(lnmax);

	if(username == NULL)
		errExit("malloc");

	printf("Username: ");
	fflush(stdin);

	if(fgets(username, lnmax, stdin) == NULL)
		exit(EXIT_FAILURE);

	len = strlen(username);
	if(username[len - 1] == '\n')
		username[len - 1] = '\0';//remove trailing \n

	pwd = getpwnam(username);
	if(pwd == NULL)
		fatal("couldn't get password record");

	spwd = getspnam(username);
	if(spwd == NULL && errno == EACCES)
		fatal("no permission to read shadow password file");

	if(spwd != NULL)
		pwd->pw_passwd = spwd->sp_pwdp;
//becase the password is in the file named shadow, and the passwd file has no passwd;

	password = getpass("Password: ");//it cannot change!
	encrypted = crypt(password, pwd->pw_passwd);

	printf("%s\n", encrypted);
	printf("%s\n", pwd->pw_passwd);
	for(p = password; *p != '\0';)
		*p++ = '\0';

	if(encrypted == NULL)
		errExit("crypt");

	authOK = strcmp(encrypted, pwd->pw_passwd) == 0;
	if(!authOK){
		printf("Incorrect password\n");
		exit(EXIT_FAILURE);
	}

	printf("Successfully authenticated: UID = %ld\n", (long)pwd->pw_uid);

	exit(EXIT_SUCCESS);
}
