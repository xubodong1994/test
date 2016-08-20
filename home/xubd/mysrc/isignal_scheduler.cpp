#include "isignal_scheduler.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <cerrno>
#include <cstring>
#include "iioevent_handler.h"
#include "ievent_reactor.h"

static int _S_signal_fd = 0;

void _S_signal_handler(int _sig_num)
{
  unsigned char _msg = (unsigned char)_sig_num;
  send(_S_signal_fd, &_msg, sizeof(_msg), 0);
}

ISignalScheduler::ISignalScheduler(IEventReactor *_reactor)
{
  ISignalDispatchExecute *  _dispatch_exec;
  IIOEventHandler        *  _ioev_handler;

  _M_reactor = _reactor;
  if (socketpair(AF_INET, SOCK_STREAM, 0, _M_signal_pair) < 0)
    std::cerr << "ERROR syscall socketpair " << strerror(errno) 
      << std::endl;
  memset(_M_signal_handlers, 0, sizeof(_M_signal_handlers));
  _M_signal_handler_num = 0;
  _M_tmp_signals = new char[2048];
  if (_M_tmp_signals == NULL)
    std::cerr << "ERROR allocate memory failed" << std::endl;
  _M_tmp_signals[0] = 0;


  _dispatch_exec  = new ISignalDispatchExecute(this);
  _ioev_handler   = new IIOEventHandler(_M_signal_pair[1], _reactor);
  if (_dispatch_exec == NULL || _ioev_handler == NULL)
    std::cerr << "ERROR allocate memory failed" << std::endl;
  _ioev_handler->set_exec(_dispatch_exec, NULL, NULL);
  _ioev_handler->enable(IHandlerType::IHNDL_READ);
}

int ISignalScheduler::signals(const char *_signals, size_t size)
{
  if (size > 2048) {
    size = 2048;
    std::cout << "WARNING too many signals" << std::endl;
  }

  memcpy(_M_tmp_signals, _signals, size);
  _M_tmp_signals[size] = 0;
  return 0;
}

ISignalScheduler::~ISignalScheduler()
{
  _M_reactor = NULL;
  (void)close(_M_signal_pair[0]);
  (void)close(_M_signal_pair[1]);
  memset(_M_signal_handlers, 0, sizeof(_M_signal_handlers));
  _M_signal_handler_num = 0;
}

bool ISignalScheduler::poolEmpty() const
{
  return _M_signal_handler_num == 0;
}

std::string ISignalScheduler::toString() const
{
  return std::string("ISignalScheduler");
}

bool ISignalScheduler::schedule(ISignalHandler *_sig_handler)
{
  bool              _ret_code;
  int               _sig_num;
  ISignalHandler *  _old_handler;

  if (_sig_handler == NULL) {
    std::cerr << "ERROR schedule an null signal handler" << std::endl;
    _ret_code = false;
  }
  else {
    _sig_num = _sig_handler->signal_number();
    _old_handler = _M_signal_handlers[_sig_num];
    _S_signal_fd = _M_signal_pair[0];
    if (_old_handler == _sig_handler) {
      std::cerr << "WARNING re-schedule signal handler" << std::endl;
      _ret_code = true;
    }
    else if (_M_install_sig_handler(_sig_num, _S_signal_handler) < 0) {
      _ret_code = false;
    }
    else {
      if (_old_handler == NULL)
        ++_M_signal_handler_num;
      _M_signal_handlers[_sig_num] = _sig_handler;
      _ret_code = true;
    }
  }

  return _ret_code;
}

bool ISignalScheduler::cancel(ISignalHandler *_sig_handler)
{
  bool          _ret_code;
  int           _sig_num;

  if (_sig_handler == NULL) {
    std::cerr << "ERROR cancel an null signal handler" << std::endl;
    _ret_code = false;
  }
  else {
    _sig_num = _sig_handler->signal_number();
    if (_M_signal_handlers[_sig_num] != _sig_handler) {
      std::cerr << "WARNING cancel handler which is not scheduled"
        << std::endl;
      _ret_code = false;
    }
    else if (_M_restore_sig_handler(_sig_num) < 0) {
      _ret_code = false;
    }
    else {
      _M_signal_handlers[_sig_num] = NULL;
      --_M_signal_handler_num;
      _ret_code = true;
    }
  }

  return _ret_code;
}

int ISignalScheduler::_M_install_sig_handler(int _sig_num,
  void (*_sig_handler)(int))
{
  int               _ret_code;
  struct sigaction  _sa;

  if (_sig_num >= _S_signal_max) {
    std::cerr << "ERROR parameter in _M_install_sig_handler, "
      << "_sig_num is " << _sig_num << std::endl;
    _ret_code = -1;
  }
  else if (_sig_handler == NULL) {
    std::cerr << "ERROR parameter in _M_install_sig_handler, "
      << "_sig_handler is null" << std::endl;
    _ret_code = -1;
  }
  else {
    memset(&_sa, 0, sizeof(_sa));
    _sa.sa_handler = _sig_handler;
    _sa.sa_flags |= SA_RESTART;
    sigfillset(&_sa.sa_mask);
    _ret_code = 0;

    if (sigaction(_sig_num, &_sa, NULL) < 0) {
      std::cerr << "ERROR syscall sigaction " << strerror(errno)
        << std::endl;
      _ret_code = -1;
    }
  }

  return _ret_code;
}

bool ISignalScheduler::dispatch(const struct timeval *_tvp)
{
  bool        _ret_code = false;
  int         _i = 0;
  int         _sig_num;
  int         _signal_cnt[_S_signal_max] = { 0 };
  while (_M_tmp_signals[_i] != 0) {
    _sig_num = _M_tmp_signals[_i];
    ++_signal_cnt[_sig_num];
  }

  for (_i = 0; _i < _S_signal_max; ++_i) {
    if (_M_signal_handlers[_i] != NULL) {
      _M_reactor->activate(_M_signal_handlers[_i]);
    }
    else {
      std::cerr << "ERROR in ISignalScheduler::dispatch "
        << "null handlers for signal " << _i << " in ISignalScheduler, " 
        << "but get it from io handler" << std::endl;
    }
  }
  return _ret_code;
}

int ISignalScheduler::_M_restore_sig_handler(int _sig_num)
{
  int              _ret_code;
  struct sigaction _sa;

  memset(&_sa, 0, sizeof(_sa));
  _sa.sa_handler = NULL;

  if (sigaction(_sig_num, &_sa, NULL) < 0) {
    std::cerr << "ERROR syscall sigaction " << strerror(errno)
      << std::endl;
    _ret_code = -1;
  }

  return _ret_code;
}

void ISignalDispatchExecute::execute(void *_argument)
{
  ssize_t _num_read = 0;
  char _buf[2048] = { 0 };
  IIOEventHandler *_ioev_handler;

  _ioev_handler = (IIOEventHandler *)_argument;
  if (_ioev_handler == NULL) {
    std::cerr << "ERROR in ISignalDispatchExecute::execute "
      << "null argument " << std::endl;
    return ;
  }

  _num_read = _ioev_handler->read(_buf, sizeof(_buf));
  if (_num_read == 0) {
    std::cerr << "ERROR in ISignalDispatchExecute::execute "
      << "read 0 byte from pipe " << std::endl;
    return ;
  }

  if (_num_read > 0) {
    _M_signal_scheduler->signals(_buf, _num_read);
    _M_signal_scheduler->dispatch(NULL);
  }
}

