/*************************************************************************
    > File Name: get_num.h
    > Author: 
    > Mail: 
    > Created Time: 2015年12月05日 星期六 11时08分34秒
 ************************************************************************/

#ifndef GET_NUM_H
#define GET_NUM_H

#define GN_NONNEG 01
#define GN_GT_O 02

#define GN_ANY_BASE 0100
#define GN_BASE_8 0200
#define GN_BASE_16 0400

long  getLong(const char * arg,int flags, const char *name);
int getInt(const char * arg, int flags, const char *name);

#endif
