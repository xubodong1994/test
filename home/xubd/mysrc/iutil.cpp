#include "iutil.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iostream>

void 
timer_correct(struct timeval *tv)
{
  if (tv->tv_usec < 0) {
    --tv->tv_sec;
    tv->tv_usec += USECS_PER_SEC;
    return ;
  }
  else {
    tv->tv_sec += tv->tv_usec / USECS_PER_SEC;
    tv->tv_usec = tv->tv_usec % USECS_PER_SEC;
  }
}

#ifndef _HAVE_TIMER_ADD

void
timeradd(struct timeval *a, struct timeval *b, struct timeval *res)
{
  res->tv_sec = a->tv_sec + b->tv_sec;
  res->tv_usec = a->tv_usec + b->tv_usec;
  timer_correct(res);
}

void
timersub(struct timeval *a, struct timeval *b, struct timeval *res)
{
  res->tv_sec = a->tv_sec - b->tv_sec;
  res->tv_usec = a->tv_usec - b->tv_usec;
  timer_correct(res);
}

void
timerclear(struct timeval *tvp)
{
  tvp->tv_sec = tvp->tv_usec = 0;
}

bool
timerisset(struct timeval *tvp)
{
  return tvp->tv_sec != 0 || tvp->tv_usec != 0;
}

#endif  // end _HAVE_TIMER_ADD

suseconds_t
timer2usec(const struct timeval *tvp)
{
  return tvp->tv_usec + tvp->tv_sec * USECS_PER_SEC;
}

int
timer2msec(const struct timeval *_tvp)
{
  return _tvp->tv_sec * 1000 + _tvp->tv_usec / 1000;
}

std::string
timer2string(const struct timeval *tvp)
{
  std::string sObjDesc;
  char buf[128];
  snprintf(buf, sizeof(buf), "{tv_sec = %ld, tv_usec = %d}", tvp->tv_sec, tvp->tv_usec);
  sObjDesc = buf;
  return sObjDesc;
}

void
vector2string(std::vector<IObject*> &vObjs, std::string &sObjDesc)
{
  char buf[1024];
  sObjDesc += "[";
  for (size_t i = 0; i < vObjs.size(); ++i)
    sObjDesc += vObjs[i]->toString();
  sObjDesc += "]";
}

int
n_bytes_readable(int _sock_fd)
{
#if defined(FIONREAD) && defined(WIN32)
  long _n_readable = 0;
  if (ioctlsocket(_sock_fd, FIONREAD, &_n_readable) < 0)
    return -1;
#elif defined(FIONREAD)
  int _n_readable = 0;
  if (ioctl(_sock_fd, FIONREAD, &_n_readable) < 0)
    return -1;
#endif
  return (int)_n_readable;
}

/*
 * Nonfatal error related to a system call.
 * Print a message and return.
 */
void
err_ret(const char *fmt, ...)
{
  va_list ap;

  int error;
#ifdef WIN32
  error = GetLastError();
#else
  error = errno;
#endif

  va_start(ap, fmt);
  err_doit(1, error, fmt, ap);
  va_end(ap);
}


/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void
err_sys(const char *fmt, ...)
{
  va_list ap;

  int error;
#ifdef WIN32
  error = GetLastError();
#else
  error = errno;
#endif

  va_start(ap, fmt);
  err_doit(1, error, fmt, ap);
  va_end(ap);
  exit(1);
}

/*
 * Fatal error unrelated to a system call.
 * Error code passed as explict parameter.
 * Print a message and terminate.
 */
void
err_exit(int error, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(1, error, fmt, ap);
  va_end(ap);
  exit(1);
}

/*
 * Fatal error related to a system call.
 * Print a message, dump core, and terminate.
 */
void
err_dump(const char *fmt, ...)
{
  va_list ap;

  int error;
#ifdef WIN32
  error = GetLastError();
#else
  error = errno;
#endif

  va_start(ap, fmt);
  err_doit(1, error, fmt, ap);
  va_end(ap);
  abort(); /* dump core and terminate */
  exit(1); /* shouldn't get here */
}

/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void
err_msg(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(0, 0, fmt, ap);
  va_end(ap);
}

/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void
err_quit(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  err_doit(0, 0, fmt, ap);
  va_end(ap);
  exit(1);
}

/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag".
 */
void
err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
  char buf[MAXLINE];
  vsnprintf(buf, MAXLINE, fmt, ap);
  if (errnoflag)
#ifdef WIN32
  {
    DWORD  _flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                    FORMAT_MESSAGE_IGNORE_INSERTS | 
                    FORMAT_MESSAGE_FROM_SYSTEM;
    FormatMessage(_flags, NULL, error, 0, buf, 0, NULL);
  }
#else // UNIX system
    snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s", strerror(error));
#endif
#ifdef __cplusplus // c++
  std::cout << buf << std::endl;
#else // c
  strcat(buf, "\n");
  fflush(stdout); /* in case stdout and stderr are the same */
  fputs(buf, stderr);
  fflush(NULL); /* flushes all stdio output streams */
#endif
}

/*
void
err_msg(const char *_err_msg) 
{
#ifdef WIN32
  HLOCAL _local_address=NULL;
  DWORD  _flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS | 
                  FORMAT_MESSAGE_FROM_SYSTEM;
  FormatMessage(_flags, NULL, GetLastError(), 0, (LPSTR)&_local_address, 0, NULL);
  std::cerr << _err_msg << ' ' << _local_address << std::endl;
#else
  std::cerr << _err_msg << ' ' << strerror(errno) << std::endl;
#endif
}
*/

