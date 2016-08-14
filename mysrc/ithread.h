#ifndef __ITHREAD_H
#define __ITHREAD_H
#define HAVE_PTHREAD

#ifdef WIN32
#include <windows.h>
#include <afxmt.h>
#elif defined(HAVE_PTHREAD)
#include <pthread.h>
#include <semaphore.h>
#endif

class IMutexLock {
public:
  IMutexLock();
  ~IMutexLock();
  int lock();
  int unlock();
  int trylock();
  void *underlying_handle() const {
    return (void *)&_M_mutex;
  }

private:
#ifdef  WIN32
  CRITICAL_SECTION      _M_mutex;
#elif defined(HAVE_PTHREAD)
  pthread_mutex_t       _M_mutex;
  pthread_mutexattr_t   _M_mutexattr;
#endif
};

class ISemaphore {
public:
  ISemaphore(const char *_name, int _init_value);
  ~ISemaphore();
  int post();
  int wait(const struct timeval *_tvp);

private:
#ifdef WIN32
  HANDLE    _M_sem;
#elif defined(HAVE_PTHREAD)
  sem_t *   _M_sem;
#endif
};

class IConditionVariable {
public:
  IConditionVariable();
  ~IConditionVariable();
  int wait(void *_lock, const struct timeval *_tvp);
  int signal();
  int broadcast();

#ifdef WIN32
  HANDLE            _M_event;
  CRITICAL_SECTION  _M_mutex;
  int               _M_num_waiting;
  int               _M_num_to_wake;
  int               _M_generation;
#elif defined(HAVE_PTHREAD)
  pthread_cond_t    _M_cond;
#endif
};

class IAutoLock {
public:
  IAutoLock(IMutexLock &_lock) : _M_lock(_lock) {
    _M_lock.lock();
  }

  ~IAutoLock() {
    _M_lock.unlock();
  }
private:
  IMutexLock &_M_lock;
};

#endif  // end of __ITHREAD_H

