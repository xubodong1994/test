/*************************************************************************
    > File Name: error_function.h
    > Author: 
    > Mail: 
    > Created Time: 2015年12月04日 星期五 10时50分22秒
 ************************************************************************/

#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H

void errMsg(const char *format, ...);

#ifdef __GNUC__
#define NORETURN __attribute__((__noreturn__))
#else
#define NORETURN
#endif
void errExit(const char *format, ...) NORETURN;
void err_exit(const char *format, ...) NORETURN;
void errExitEN(int errnum,const char *format, ...) NORETURN;
void fatal(const char *format, ...) NORETURN;
void usageErr(const char *format, ...) NORETURN;
void cmdLineErr(const char *format, ...) NORETURN;
#endif

//use the top three function to judge the systerm function
