/*************************************************************************
    > File Name: error_functions.c
    > Author: 
    > Mail: 
    > Created Time: 2015年12月05日 星期六 08时31分36秒
 ************************************************************************/

#include<stdarg.h>
#include"error_functions.h"
#include"tlpi_hdr.h"
#include"ename.c.inc"

#ifdef __GNUC__
__attribute__((__noreturn__))
#endif
//dump core mean that the damon can not response,and then record the
//register's value
static void 
terminate(Boolean useExit3)
{
	char *s;
	s = getenv("EF_DUMPCORE");
	if(s != NULL && *s != '\0')
		abort();
	else if(useExit3)
		exit(EXIT_FAILURE);
	else 
		_exit(EXIT_FAILURE);
}

static void 
outputError(Boolean useErr, int err, Boolean flushStdout,const char *format, va_list ap)
{
#define BUF_SIZE 500
	char buf[BUF_SIZE], userMsg[BUF_SIZE], errText[BUF_SIZE];
	vsnprintf(userMsg,BUF_SIZE,format,ap);
	//int _vsnprintf(char* str, size_t size, const char* format, va_list ap);
	//参数说明:
	//
	//    char *str [out],把生成的格式化的字符串存放在这里.
	//        size_t size [in], str可接受的最大字节数,防止产生数组越界.
	//            const char *format [in], 指定输出格式的字符串，它决定了你需要提供的可变参数的类型、个数和顺序。
	//                va_list ap [in], va_list变量. va:variable-argument:可变参数
	//
	//                函数功能：将可变参数格式化输出到一个字符数组。
	//                用法类似于vsprintf，不过加了size的限制，防止了内存溢出（size为str所指的存储空间的大小）。
	//                返回值：执行成功，返回写入到字符数组str中的字符个数（不包含终止符），最大不超过size；执行失败，返回负值，并置errno.[1]  
	if(useErr)
		snprintf(errText, BUF_SIZE, "[%s %s]",(err > 0 && err <= MAX_ENAME)?ename[err] : "?UNKNOWN?", strerror(err));
	// snprintf(char *str, size_t size, const char *format, ...);
	// strerror:通过标准错误的标号，获得错误的描述字符串 ，将单纯的错误标号转为字符串描述，方便用户查找错误。
	// 将可变个参数(...)按照format格式化成字符串，然后将其复制到str中
	// (1) 如果格式化后的字符串长度 < size，则将此字符串全部复制到str中，并给其后添加一个字符串结束符('\0')；
	// (2) 如果格式化后的字符串长度 >= size，则只将其中的(size-1)个字符复制到str中，并给其后添加一个字符串结束符('\0')，返回值为欲写入的字符串长度。'')'')
	else
		snprintf(errText, BUF_SIZE, ":");

	snprintf(buf,BUF_SIZE, "ERROR%s %s\n", errText,userMsg);

	if(flushStdout)
		fflush(stdout);
	fputs(buf,stderr);
}

void errMsg(const char *format, ...)
{
	va_list argList;
	int savedErrno;
	savedErrno = errno;
	va_start(argList,format);
	outputError(TRUE, errno, TRUE, format,argList);
	va_end(argList);
	errno = savedErrno;
}

void errExit(const char *format,...)
{
	va_list argList;
	va_start(argList,format);
	outputError(TRUE,errno, TRUE, format,argList);
	va_end(argList);
	terminate(TRUE);
}

void err_exit(const char *format, ...)//use _exit quit, so never fflush!
{
	va_list argList;
	va_start(argList,format);
	outputError(TRUE, errno, FALSE, format, argList);
	va_end(argList);
	terminate(FALSE);
}

void errExitEN(int errnum, const char * format,...)
{
	va_list argList;
	va_start(argList,format);
	outputError(TRUE, errnum, TRUE, format, argList);
	va_end(argList);
	terminate(TRUE);
}

void fatal(const char * format, ...)//for the universal/general
{
	va_list argList;
	va_start(argList, format);
	outputError(FALSE, 0, TRUE, format, argList);
	va_end(argList);
	terminate(TRUE);
}

void usageErr(const char *format,...)//for the arg 
{
	va_list argList;
	fflush(stdout);
	fprintf(stderr, "Usage:");
	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);

	fflush(stderr);
	exit(EXIT_FAILURE);
}

void cmdLineErr(const char * format, ...)//for commad line arg;
{
	va_list argList;
	fflush(stdout);
	fprintf(stderr,"Commad-line, usage error:");
	va_start(argList,format);
	vfprintf(stderr, format, argList);
	va_end(argList);
	fflush(stderr);
	exit(EXIT_FAILURE);
}

