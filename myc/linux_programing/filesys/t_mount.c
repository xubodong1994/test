/*************************************************************************
    > File Name: t_mount.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月17日 星期四 21时45分49秒
 ************************************************************************/

#include<sys/mount.h>
#include"tlpi_hdr.h"

static void 
usageError(const char* progName, const char *msg)
{
	if(msg != NULL)
		fprintf(stderr, "%s", msg);
	fprintf(stderr, "Usage: %s[options] source target\n\n",progName);
	fprintf(stderr, "Available options:\n");
#define fpe(str) fprintf(stderr, "		"str)
	fpe("-t fstype		[e.g., 'ext2' or 'reiserfs']\n");
	fpe("-o data		[file system-dependent options,\n");
	fpe("				e.g., 'bsdgroups' for ext2]\n");
	fpe("-f mountflags can include any of:\n");
#define fpe2(str) fprintf(stderr, "				" str)
	fpe2("b - MS_BIND		create a bind mount\n");
	fpe2("d - MSDIRSYNC		synchronous directory updates\n");
	fpe2("l - MS_MANDLOCK	permit mandatory locking\n");
	fpe2("m - MS_MOVE		automically move subtree\n");
	fpe2("A - MS_NOATIME	don't update atime (last access time)\n");
	fpe2("V - MS_NODEV		don't  permit device access\n");

}

