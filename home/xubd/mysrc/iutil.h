#ifndef __IUTIL_H
#define __IUTIL_H
#include <string>
#include <sys/time.h>
#include <vector>
#include "iobject.h"
#include <sys/ioctl.h>
#include <stdarg.h>

#define USECS_PER_SEC 1000000

#ifndef FD_COPY
#define FD_COPY(_orig_set, _dest_set) \
  memcpy(_dest_set, _orig_set, sizeof(*_dest_set));
#endif

#ifndef MAXLINE
#define MAXLINE (1024 * 1024)
#endif

extern
void timer_correct(struct timeval *t);

#ifndef _HAVE_TIMER_ADD

extern 
void timeradd(struct timeval *a, struct timeval *b, struct timeval *res);

extern 
void timersub(struct timeval *a, struct timeval *b, struct timeval *res);

extern  
void timerclear(struct timeval *tvp);

extern  
void timerisset(struct timeval *tvp);

#define timercmp(a, b, cmp) \
  ( \
    ((a)->tv_sec == (b)->tv_sec) ? \
    ((a)->tv_usec cmp (b)->tv_usec) : \
    ((a)->tv_sec cmp (b)->tv_sec) \
  )

#endif  // end _HAVE_TIMER_ADD

extern
suseconds_t timer2usec(const struct timeval *tvp);

extern
int timer2msec(const struct timeval *_tvp);

extern
std::string timer2string(const struct timeval *tvp);

extern void 
vector2string(const std::vector<IObject> &, std::string &sObjDesc);

extern int
n_bytes_readable(int sockfd);

/*
 * Nonfatal error related to a system call.
 * Print a message and return.
 */
extern void
err_ret(const char *fmt, ...);

/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
extern void
err_sys(const char *fmt, ...);

/*
 * Fatal error unrelated to a system call.
 * Error code passed as explict parameter.
 * Print a message and terminate.
 */
extern void
err_exit(int error, const char *fmt, ...);

/*
 * Fatal error related to a system call.
 * Print a message, dump core, and terminate.
 */
extern void
err_dump(const char *fmt, ...);

/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
extern void
err_msg(const char *fmt, ...);

/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
extern void
err_quit(const char *fmt, ...);

/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag".
 */
extern void
err_doit(int errnoflag, int error, const char *fmt, va_list ap);

#endif  // end __IUTIL_H

