#include "iioevent_handler.h"
#include "iioevent_handler.h"
#include "ievent_reactor.h"
#include "iutil.h"
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <iostream>

IIOTimerHandler::IIOTimerHandler(IEventHandler *_handler)
  : ITimerHandler(_handler->reactor())
{
  //_M_relate_io_handler = _handler;
  argument((void *)_handler);
}

void IIOTimerHandler::run()
{
  IEventHandler *_relate_io_handler = NULL;
  if (argument() != NULL) {
    _relate_io_handler = (IEventHandler *)argument();
    _relate_io_handler->run();
  }
}

IIOTimerHandler::~IIOTimerHandler()
{
}

ICommonReadHandler::ICommonReadHandler(int _fd, IEventReactor *_r, 
  struct timeval tvInterval, IIOEventHandler *_ioev_handler)
  : IReadHandler(_fd, _r), _M_read_timer_handler(NULL)
{
  timerclear(&_M_read_interval);
  if (timerisset(&tvInterval)) {
    _M_read_interval = tvInterval;
    _M_read_timer_handler = new IIOTimerHandler(this);
  }
  else {
    _M_read_timer_handler = NULL;
  }
  _M_buffer = new IEventBuffer;
  _M_ioev_handler = _ioev_handler;
  if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
    std::cout << "ERROR syscall fcntl " << strerror(errno) << std::endl;
  }
}

IEventBuffer *ICommonReadHandler::buffer() const
{
  return _M_buffer;
}

struct timeval ICommonReadHandler::interval() const
{
  return _M_read_interval;
}

void ICommonReadHandler::enable()
{
  std::cout << "ICommonReadHandler::enable fd " << fd() << std::endl;
  reactor()->schedule(this, (void *)_M_ioev_handler);
  if (_M_read_timer_handler != NULL)
    reactor()->schedule(_M_read_timer_handler, &_M_read_interval, this);
}

void ICommonReadHandler::disable()
{
  std::cout << "ICommonReadHandler::disable() fd " << fd() << std::endl;
  reactor()->cancel(this);
  if (_M_read_timer_handler != NULL)
    reactor()->cancel(_M_read_timer_handler);
}

void ICommonReadHandler::run()
{
  ssize_t           _num_read = 0;
  IIOEventHandler * _ioev_handler = NULL;
  bool              _fatal_err = false;

  std::cout << "ICommonReadHandler::run()" << std::endl;

  _ioev_handler = (IIOEventHandler *)argument();
  if (_ioev_handler == NULL)
    _fatal_err = true;

  if (_M_buffer == NULL)
    if ((_M_buffer = new IEventBuffer) == NULL)
      _fatal_err = true;

  while (!_fatal_err) {
    _num_read = _M_buffer->read(fd(), 4096);
    if (_num_read > 0 && _num_read < 4096) {
      std::cout << "read " << _num_read << " bytes from " << fd() << std::endl;
      break;
    }
    if (_num_read < 0) {
      if (errno != EAGAIN && errno != EINTR) {
        _fatal_err = true;
        break;
      }
      else if (errno == EAGAIN)
        break;
    }
    else if (_num_read == 0)
      break;
  }

  if (_fatal_err) {
    _ioev_handler->run_error_exec();
    return ;
  }
  else {
    if (_M_read_timer_handler != NULL) {
      struct timeval _io_interval = interval();
      reactor()->cancel(_M_read_timer_handler);
      reactor()->schedule(_M_read_timer_handler, &_io_interval, (void *)this);
    }
    if (m_pExec != NULL)
      m_pExec->execute((void *)_ioev_handler);
  }
  std::cout << "ICommonReadHandler::run() end" << std::endl;
}

ICommonReadHandler::~ICommonReadHandler()
{
  if (_M_buffer != NULL) {
    delete _M_buffer;
    _M_buffer = NULL;
  }

  if (_M_read_timer_handler) {
    reactor()->cancel(_M_read_timer_handler);
    _M_read_timer_handler = NULL;
  }

  timerclear(&_M_read_interval);
}

ICommonWriteHandler::ICommonWriteHandler(int _fd, IEventReactor *_r,
  struct timeval tvInterval, IIOEventHandler *_ioev_handler)
  : IWriteHandler(_fd, _r)
{
  timerclear(&_M_write_interval);
  if (timerisset(&tvInterval)) {
    _M_write_interval = tvInterval;
    _M_write_timer_handler = new IIOTimerHandler(this);
  }
  else {
    _M_write_timer_handler = NULL;
  }
  _M_buffer = new IEventBuffer;
  _M_ioev_handler = _ioev_handler;
  if (fcntl(_fd, F_SETFL, O_NONBLOCK) < 0) {
    std::cout << "ERROR syscall fcntl " << strerror(errno) << std::endl;
  }
}

IEventBuffer *ICommonWriteHandler::buffer() const
{
  return _M_buffer;
}

struct timeval ICommonWriteHandler::interval() const
{
  return _M_write_interval;
}

void ICommonWriteHandler::enable()
{
  std::cout << "ICommonWriteHandler::enable fd " << fd() << std::endl;
  reactor()->schedule(this, (void *)_M_ioev_handler);
  if (_M_write_timer_handler != NULL)
    reactor()->schedule(_M_write_timer_handler, &_M_write_interval, this);
}

void ICommonWriteHandler::disable()
{
  std::cout << "ICommonWriteHandler::disable fd " << fd() << std::endl;
  reactor()->cancel(this);
  if (_M_write_timer_handler != NULL)
    reactor()->cancel(_M_write_timer_handler);
}

void ICommonWriteHandler::run()
{
  ssize_t           _num_wrote = 0;
  IIOEventHandler * _ioev_handler = NULL;
  bool              _fatal_err = false;

  _ioev_handler = (IIOEventHandler *)argument();
  if (_ioev_handler == NULL)
    _fatal_err = true;

  if (_M_buffer == NULL)
    if ((_M_buffer = new IEventBuffer) == NULL)
      _fatal_err = true;

  while (!_fatal_err) {
    _num_wrote = _M_buffer->write(fd(), 4096);
    std::cerr << _M_buffer->toString() << std::endl;
    if (_num_wrote > 0 && _num_wrote < 4096) {
      std::cout << "write " << _num_wrote << " bytes to " << fd() << std::endl;
      break;
    }
    if (_num_wrote < 0) {
      if (errno != EAGAIN && errno != EINTR) {
        _fatal_err = true;
        break;
      }
      else if (errno == EAGAIN)
        break;
    }
    if (_M_buffer->size() == 0)
      break;
  }

  if (_fatal_err) {
    _ioev_handler->run_error_exec();
    return ;
  }
  else {
    if (_M_write_timer_handler != NULL) {
      struct timeval _io_interval = interval();
      reactor()->cancel(_M_write_timer_handler);
      reactor()->schedule(_M_write_timer_handler, &_io_interval, (void *)this);
    }
    if (_M_buffer->size() == 0)
      _ioev_handler->disable(this->type());
    if (m_pExec != NULL)
      m_pExec->execute((void *)_ioev_handler);
  }
}

ICommonWriteHandler::~ICommonWriteHandler()
{
  if (_M_buffer != NULL) {
    delete _M_buffer;
    _M_buffer = NULL;
  }

  if (_M_write_timer_handler != NULL) {
    delete _M_write_timer_handler;
    _M_write_timer_handler = NULL;
  }

  timerclear(&_M_write_interval);
}

IIOEventHandler::IIOEventHandler(int _fd, IEventReactor *_r)
{
  struct timeval _tv_zero;
  timerclear(&_tv_zero);
  _M_read_handler   = new ICommonReadHandler(_fd, _r, _tv_zero, this);
  _M_write_handler  = new ICommonWriteHandler(_fd, _r, _tv_zero, this);
  _M_suspend_flag   = IHandlerType::IHNDL_READ | IHandlerType::IHNDL_WRITE;
}

IEventBuffer * IIOEventHandler::read_buffer() const
{
  IEventBuffer * _ret_buffer;

  _ret_buffer = NULL;
  if (_M_read_handler != NULL)
    _ret_buffer = _M_read_handler->buffer();

  return _ret_buffer;
}

IEventBuffer * IIOEventHandler::write_buffer() const
{
  IEventBuffer * _ret_buffer;

  _ret_buffer = NULL;
  if (_M_write_handler != NULL)
    _ret_buffer = _M_write_handler->buffer();

  return _ret_buffer;
}

ssize_t IIOEventHandler::read(char *_buf, int _how_much)
{
  ssize_t _num_read = -1;

  if (_M_read_handler != NULL)
    _num_read = read_buffer()->write(_buf, _how_much);

  return _num_read;
}

ssize_t IIOEventHandler::write(const char *_buf, int _how_much)
{
  ssize_t _num_wrote = 0;

  if (_M_write_handler != NULL)
    _num_wrote = write_buffer()->read(_buf, _how_much);

  if (_num_wrote > 0 && (_M_suspend_flag & IHandlerType::IHNDL_WRITE))
    enable(IHandlerType::IHNDL_WRITE);

  return _num_wrote;
}

void IIOEventHandler::enable(int _handler_type)
{
  if (_handler_type & IHandlerType::IHNDL_READ)
    if (_M_read_handler != NULL) {
      _M_read_handler->enable();
      _M_suspend_flag &= ~IHandlerType::IHNDL_READ;
    }

  if (_handler_type & IHandlerType::IHNDL_WRITE)
    if (_M_write_handler != NULL) {
      _M_write_handler->enable();
      _M_suspend_flag &= ~IHandlerType::IHNDL_WRITE;
    }
}

void IIOEventHandler::disable(int _handler_type)
{
  if (_handler_type & IHandlerType::IHNDL_READ)
    if (_M_read_handler != NULL && !(_M_suspend_flag & IHandlerType::IHNDL_READ)) {
      _M_read_handler->disable();
      _M_suspend_flag |= IHandlerType::IHNDL_READ;
    }

  if (_handler_type & IHandlerType::IHNDL_WRITE)
    if (_M_write_handler != NULL && !(_M_suspend_flag & IHandlerType::IHNDL_WRITE)) {
      _M_write_handler->disable();
      _M_suspend_flag |= IHandlerType::IHNDL_WRITE;
    }
}

void IIOEventHandler::set_exec(IExecutable *_r, IExecutable *_w, IExecutable *_e)
{
  if (_M_read_handler != NULL)
    _M_read_handler->setExecute(_r);

  if (_M_write_handler != NULL)
    _M_write_handler->setExecute(_w);

  _M_error_exec = _e;
}

void IIOEventHandler::run_error_exec()
{
  if (_M_error_exec != NULL)
    _M_error_exec->execute(this);
}

IIOEventHandler::~IIOEventHandler()
{
  if (_M_read_handler != NULL) {
    disable(IHandlerType::IHNDL_READ);
    delete _M_read_handler;
    _M_read_handler = NULL;
  }

  if (_M_write_handler != NULL) {
    disable(IHandlerType::IHNDL_WRITE);
    delete _M_write_handler;
    _M_write_handler = NULL;
  }

  _M_error_exec = NULL;

  _M_suspend_flag = 0;
}

