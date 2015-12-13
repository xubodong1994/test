/*************************************************************************
    > File Name: ugid_functions.h
    > Author: 
    > Mail: 
    > Created Time: 2015年12月10日 星期四 15时10分33秒
 ************************************************************************/
#ifndef UGID_FUNCTIONS_H
#define UGID_FUNCTIONS_H

char *userNameFromId(uid_t uid);

uid_t userIdFromName(const char *name);

char *groupNameFromId(gid_t gid);

gid_t groupIdFromName(const char *name);
#endif
