#ifndef __IIOEVENT_HANDLER_H
#define __IIOEVENT_HANDLER_H

#include "iread_handler.h"
#include "iwrite_handler.h"
#include "itimer_handler.h"
#include "ibuffer.h"
#include <sys/time.h>

class ICommonReadHandler;
class ICommonWriteHandler;
class IIOEventHandler;

class IIOTimerHandler : public ITimerHandler {
public:
  IIOTimerHandler(IEventHandler *_handler);
  virtual void run();
  ~IIOTimerHandler();
};

class ICommonReadHandler : public IReadHandler {
public:
  ICommonReadHandler(int _fd, IEventReactor *_r, struct timeval tvInterval, 
    IIOEventHandler *_ioev_handler);
  virtual void run();
  IEventBuffer *buffer() const;
  void enable();
  void disable();
  struct timeval interval() const;
  ~ICommonReadHandler();
private:
  IIOTimerHandler * _M_read_timer_handler;
  struct timeval    _M_read_interval;
  IEventBuffer *    _M_buffer;
  IIOEventHandler * _M_ioev_handler;
};

class ICommonWriteHandler : public IWriteHandler {
public:
  ICommonWriteHandler(int _fd, IEventReactor *_r, struct timeval tvInterval,
    IIOEventHandler *_ioev_handler);
  virtual void run();
  IEventBuffer *buffer() const;
  void enable();
  void disable();
  struct timeval interval() const;
  ~ICommonWriteHandler();
private:
  IIOTimerHandler * _M_write_timer_handler;
  struct timeval    _M_write_interval;
  IEventBuffer *    _M_buffer;
  IIOEventHandler * _M_ioev_handler;
};

class IIOEventHandler {
public:
  IIOEventHandler(int _fd, IEventReactor *_reactor);
  IEventBuffer *read_buffer() const;
  IEventBuffer *write_buffer() const;
  ssize_t read(char *_buf, int _how_much);
  ssize_t write(const char *_buf, int _how_much);
  void enable(int _handler_type);
  void disable(int _handler_type);
  void set_exec(IExecutable *_read_exec, IExecutable *_write_exec, IExecutable *_err_exec);
  void run_error_exec();
  ~IIOEventHandler();

private:
  ICommonReadHandler *  _M_read_handler;
  ICommonWriteHandler * _M_write_handler;
  IExecutable *         _M_error_exec;
  int                   _M_suspend_flag;
};

#endif

