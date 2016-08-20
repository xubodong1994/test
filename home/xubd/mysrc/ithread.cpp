#include "ithread.h"
#include <iostream>
#include "iutil.h"
#ifdef  WIN32
#elif defined(HAVE_PTHREAD)
#include <fcntl.h>
#include <sys/stat.h>
#endif  // end of WIN32

#ifndef HAVE_PTHREAD
#define HAVE_PTHREAD
#endif  // end of WIN32
/**
 * begin IMutexLock
 */

IMutexLock::IMutexLock()
{
#ifdef  WIN32
  if (InitializeCriticalSectionAndSpinCount(&_M_mutex, 2000) != TRUE) {
    DeleteCriticalSection(&_M_mutex);
    std::cerr << "init critical section failed !" << std::endl;
  }
#elif defined(HAVE_PTHREAD)
  int       _ret_code = 0;
  if ((_ret_code = pthread_mutexattr_init(&_M_mutexattr)) != 0)
    std::cerr << "mutex attribute init failed !" << std::endl;
  if (_ret_code == 0)
    _ret_code = pthread_mutexattr_settype(&_M_mutexattr, PTHREAD_MUTEX_ERRORCHECK);
  if (_ret_code == 0)
    if ((_ret_code = pthread_mutex_init(&_M_mutex, &_M_mutexattr)) != 0)
      err_msg("pthread_mutex_init failed");

  if (_ret_code != 0) {
    (void)pthread_mutex_destroy(&_M_mutex);
    (void)pthread_mutexattr_destroy(&_M_mutexattr);
  }
#endif  // end of WIN32
}

int IMutexLock::lock()
{
  int _ret_code = 0;
#ifdef  WIN32
  EnterCriticalSection(&_M_mutex);
#elif defined(HAVE_PTHREAD)
  _ret_code = pthread_mutex_lock(&_M_mutex);
#endif  // end of WIN32
  return _ret_code;
}

int IMutexLock::trylock()
{
  int _ret_code = 0;
#ifdef  WIN32
  TryEnterCriticalSection(&_M_mutex);
#elif defined(HAVE_PTHREAD)
  _ret_code = pthread_mutex_trylock(&_M_mutex);
#endif  // end of WIN32
  return _ret_code;
}

int IMutexLock::unlock()
{
  int _ret_code = 0;
#ifdef  WIN32
  LeaveCriticalSection(&_M_mutex);
#elif defined(HAVE_PTHREAD)
  _ret_code = pthread_mutex_unlock(&_M_mutex);
#endif  // end of WIN32
  return _ret_code;
}

IMutexLock::~IMutexLock()
{
#ifdef  WIN32
  (void)DeleteCriticalSection(&_M_mutex);  
#elif defined(HAVE_PTHREAD)
  (void)pthread_mutex_destroy(&_M_mutex);
#endif  // end of WIN32
}

/**
 * end IMutexLock
 */

/**
 * begin ISemaphore
 */
ISemaphore::ISemaphore(const char *_name, int _init_value)
{
  int _max_value;

#ifdef SEM_VALUE_MAX
  _max_value = SEM_VALUE_MAX;
#else
  _max_value = 32767;
#endif

#ifdef  WIN32
  _M_sem = CreateSemaphore(NULL, _init_value, 32767, _name);
  if (_M_sem == NULL) {
    err_msg("Create Semaphore failed");
  }
#elif defined(HAVE_PTHREAD)
  _M_sem = sem_open(_name, O_CREAT, 0777, _init_value);
  if (_M_sem == NULL) {
    err_msg("sem_open failed");
  }
#endif  // end of WIN32
}

int ISemaphore::post()
{
  int _ret_code;
#ifdef  WIN32
  if (ReleaseSemaphore(_M_sem, 1, NULL) == TRUE)
    _ret_code = 0;
  else
    _ret_code = -1;
#elif defined(HAVE_PTHREAD)
  _ret_code = sem_post(_M_sem);
#endif  // end of WIN32
  return _ret_code;
}

int ISemaphore::wait(const struct timeval *_tvp)
{
  int _ret_code;
#ifdef  WIN32
  int _milli_secs;
  if (_tvp == NULL)
    _milli_secs = INFINITE;
  else
    _milli_secs = timer2msec(_tvp);
  if (WaitForSingleObject(_M_sem, _milli_secs) != 0) {
    err_msg("WaitForSingleObject failed");
  }
  _ret_code = -1;
#elif defined(HAVE_PTHREAD)
  struct timespec _ts;
  if (_tvp != NULL) {
    _ts.tv_sec  = _tvp->tv_sec;
    _ts.tv_nsec = _tvp->tv_usec * 1000;
  }

#if _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600
#define HAVE_SEM_TIMEDWAIT
#endif
#ifdef HAVE_SEM_TIMEDWAIT
  if (_tvp == NULL)
    _ret_code = sem_wait(_M_sem);
  else
    _ret_code = sem_timedwait(_M_sem, &_ts);
#else
  _ret_code = sem_wait(_M_sem);
#endif  // end of HAVE_SEM_TIMEDWAIT
#endif  // end of WIN32
  return _ret_code;
}

ISemaphore::~ISemaphore()
{
#ifdef  WIN32
  (void)CloseHandle(_M_sem);
#elif defined(HAVE_PTHREAD)
  (void)sem_close(_M_sem);
#endif  // end of WIN32
}
/**
 * end ISemaphore
 */

/**
 * begin IConditionVariable
 */

IConditionVariable::IConditionVariable()
{
#ifdef WIN32
  _M_event = CreateEvent(NULL, TRUE, FALSE, "");
  if (_M_event == NULL)
    goto error_out;
  if (InitializeCriticalSectionAndSpinCount(&_M_mutex, 2000) == FALSE) {
    (void)CloseHandle(_M_event);
    goto error_out;
  }
  _M_num_waiting = 0;
  _M_num_to_wake = 0;
  _M_generation = 0;
  goto normal_out;
#elif defined(HAVE_PTHREAD)
  if (pthread_cond_init(&_M_cond, NULL) < 0)
    goto error_out;
  goto normal_out;
#endif
error_out:
  err_msg("Init condition variable failed!");
normal_out:
  ; // nothing to do
}

int IConditionVariable::signal()
{
#ifdef WIN32
  EnterCriticalSection(&_M_mutex);
  if (_M_num_to_wake < _M_num_waiting) {
    ++_M_num_to_wake;
    ++_M_generation;
    SetEvent(_M_event);
  }
  LeaveCriticalSection(&_M_mutex);
#elif defined(HAVE_PTHREAD)
  return pthread_cond_signal(&_M_cond);
#endif
}

int IConditionVariable::broadcast()
{
#ifdef WIN32
  EnterCriticalSection(&_M_mutex);
  _M_num_to_wake = _M_num_waiting;
  ++_M_generation;
  SetEvent(_M_event);
  LeaveCriticalSection(&_M_mutex);
#elif defined(HAVE_PTHREAD)
  return pthread_cond_broadcast(&_M_cond);
#endif
}

int IConditionVariable::wait(void *_lock, const struct timeval *_tvp)
{
  IMutexLock *      _mutex_lock = (IMutexLock *)_lock;
  int               _ret_code;
  if (_mutex_lock == NULL)
    return -1;
#ifdef WIN32
  CRITICAL_SECTION *_mutex;
  ULONGLONG         _time_to_wait = INFINITE;
  ULONGLONG         _start_time;
  ULONGLONG         _end_time;
  bool              _waiting = true;
  //int               _start_generation;

  _mutex = (CRITICAL_SECTION *)_mutex_lock->underlying_handle();
  if (_tvp != NULL)
    _time_to_wait = timer2msec(_tvp);

  EnterCriticalSection(&_M_mutex);
  _start_generation = _M_generation;
  ++_M_num_waiting;
  LeaveCriticalSection(&_M_mutex);

  LeaveCriticalSection(_mutex);

  do {
    _start_time = GetTickCount64();
    _ret_code = WaitForSingleObject(_M_event, _time_to_wait);
    EnterCriticalSection(&_M_mutex);
    if (_M_num_to_wake > 0 && _start_generation != _M_generation)
      --_M_num_to_wake;
      _ret_code = 0;
      _waiting = false;
    }
    else if (_ret_code == WAIT_TIMEOUT) {
      _ret_code = 1;
      _waiting = false;
    }
    else if (_time_to_wait != INFINITE) {
      _end_time = GetTickCount64();
      if (_end_time < _start_time) {
        err_msg("fatal error in IConditionVariable::wait!");
        _waiting = false;
        _ret_code = -1;
      }
      else
        _time_to_wait -= (_end_time - _start_time);
    }

    if (_M_num_to_wake == 0)
      ResetEvent(_M_event);
    LeaveCriticalSection(&_M_mutex);
  } while (_waiting);

  EnterCriticalSection(_mutex);

  EnterCriticalSection(&_M_mutex);
  if (--_M_num_waiting)
    ResetEvent(_M_event);
  LeaveCriticalSection(&_M_mutex);

#elif defined(HAVE_PTHREAD)
  struct timespec _ts, *_tsp;
  if (_tvp != NULL) {
    _ts.tv_sec = _tvp->tv_sec;
    _ts.tv_nsec = _tvp->tv_usec * 1000;
    _tsp = &_ts;
  }
  else
    _tsp = NULL;
  pthread_mutex_t *_mutex = (pthread_mutex_t *)_mutex_lock->underlying_handle();
  if (_tsp != NULL)
    _ret_code = pthread_cond_timedwait(&_M_cond, _mutex, _tsp);
  else
    _ret_code = pthread_cond_wait(&_M_cond, _mutex);
#endif
  return _ret_code;
}

IConditionVariable::~IConditionVariable()
{
#ifdef WIN32
  (void)CloseHandle(_M_event);
  (void)DeleteCriticalSection(&_M_mutex);
  _M_num_waiting = 0;
  _M_num_to_wake = 0;
  _M_generation  = 0;
#elif defined(HAVE_PTHREAD)
  pthread_cond_destroy(&_M_cond);
#endif
}

/**
 * end IConditionVariable
 */

