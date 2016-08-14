#ifndef __ISIGNAL_SCHEDULER_H
#define __ISIGNAL_SCHEDULER_H

#include "ischeduler.h"
#include "isignal_handler.h"
#include <signal.h>

class IEventReactor;
typedef void (*ISigFunctor)(int);
class ISignalScheduler : public IScheduler {
public:
  ISignalScheduler(IEventReactor *_reactor);
  bool schedule(ISignalHandler *_sig_handler);
  bool cancel(ISignalHandler *_sig_handler);
  bool poolEmpty() const;
  virtual bool dispatch(const struct timeval *_tvp);
  virtual std::string toString() const;
  virtual ~ISignalScheduler();
  int signals(const char *_signals, size_t size);

private:
  const static int  _S_signal_max = 64;
  int               _M_signal_pair[2];
  ISignalHandler *  _M_signal_handlers[_S_signal_max];
  IEventReactor *   _M_reactor;
  int               _M_signal_handler_num;
  //ISigFunctor       _M_signal_functors[_S_signal_max];
  char *            _M_tmp_signals;

private:
  int _M_install_sig_handler(int _sig_num, 
    void (*_sig_handler)(int));
  int _M_restore_sig_handler(int _sig_num);
};

class ISignalDispatchExecute : public IExecutable {
public:
  ISignalDispatchExecute(ISignalScheduler *_scheduler) {
    _M_signal_scheduler = _scheduler;
  }
  virtual std::string toString() const {
    return std::string("ISignalDispatchExec");
  }
  virtual void execute(void *_argument);
  virtual ~ISignalDispatchExecute() { _M_signal_scheduler = NULL; }

private:
  ISignalScheduler *_M_signal_scheduler;
};

#endif  // end of __ISIGNAL_SCHEDULER_H

