#include "iio_epoll_selector.h"
#ifdef HAVE_SYS_EPOLL
#include "ievent_reactor.h"
#include <sys/epoll.h>
#include <cerrno>
#include <iostream>
#include <cstring>

#if defined(MAX_FILES)
#undef MAX_FILES
#endif
#define MAX_FILES 65535

#if defined(LOG_FUNCTION)
#undef LOG_FUNCTION
#endif
#define LOG_FUNCTION \
  std::cout << __FUNCTION__ << std::endl

IIOEpollSelector::IIOEpollSelector(IEventReactor *_reactor)
{
  LOG_FUNCTION;
  _M_reactor = _reactor;

  _M_epoll_events = new struct epoll_event[128];
  if (_M_epoll_events == NULL)
    std::cerr << "ERROR alloc memory for epoll_event " << std::endl;
  else
    memset(_M_epoll_events, 0, 128 * sizeof(*_M_epoll_events));

  _M_epoll_fd = epoll_create(MAX_FILES);
  if (_M_epoll_fd < 0)
    std::cerr << "ERROR syscall epoll_create " 
      << strerror(errno) << std::endl;

  _M_events_type = new int[MAX_FILES];
  if (_M_events_type == NULL)
    std::cerr << "ERROR alloc memory for events type " << std::endl;
  else
    memset(_M_events_type, 0, MAX_FILES * sizeof(int));

  _M_num_events = 128;
}

std::string IIOEpollSelector::toString() const
{
  return std::string("IIOEpollSelector");
}

bool IIOEpollSelector::_M_prepare_epoll_event(struct epoll_event *_evp,
  int _fd, int _handler_type)
{
  LOG_FUNCTION;
  int _handler_type_all = IHandlerType::IHNDL_READ | 
                          IHandlerType::IHNDL_WRITE;
  if ((_handler_type & _handler_type_all) == 0) {
    std::cerr << "handler type " << _handler_type << " unknown " 
      << std::endl;
    return false;
  }

  //__assert((_ev != NULL));
  memset(_evp, 0, sizeof(*_evp));
  _evp->data.fd = _fd;
  //_evp->events |= EPOLLET;

  if (_handler_type & IHandlerType::IHNDL_READ)
    _evp->events |= EPOLLIN;

  if (_handler_type & IHandlerType::IHNDL_WRITE)
    _evp->events |= EPOLLOUT;

  return true;
}

bool IIOEpollSelector::add(int _fd, int _handler_type)
{
  std::string _handler_type_str;
  if (_handler_type == IHandlerType::IHNDL_READ)
    _handler_type_str = "IHNDL_READ";
  else if (_handler_type == IHandlerType::IHNDL_WRITE)
    _handler_type_str = "IHNDL_WRITE";
  else if (_handler_type == IHandlerType::IHNDL_READ | IHandlerType::IHNDL_WRITE)
    _handler_type_str = "IHNDL_READ | IHNDL_WRITE";

  std::cout << "add(" << _fd << ", " << _handler_type_str << ")" << std::endl;
  struct epoll_event  _ev;
  int                 _ev_type_orig;
  int                 _ev_type;
  int                 _op;

  if (!_M_prepare_epoll_event(&_ev, _fd, _handler_type))
    return false;

  _ev_type_orig = _M_events_type[_fd];
  _ev_type      = _ev.events;
  if (_ev_type_orig == 0) {
    _op = EPOLL_CTL_ADD;
  }
  else if ((_ev_type & _ev_type_orig) == 0) {
    _op = EPOLL_CTL_MOD;
    _ev.events |= _ev_type_orig;
  }
  else {
    std::cerr << "ERROR epoll add registed type " << _ev_type_orig 
      << ", to registe type " << _ev_type << std::endl;
    return false;
  }

  _ev.events |= EPOLLET;
  if (epoll_ctl(_M_epoll_fd, _op, _fd, &_ev) < 0) {
    std::cout << "ERROR syscall epoll_ctl in add " 
      << strerror(errno) << std::endl;
    return false;
  }

  _M_events_type[_fd] = (_ev_type_orig | _ev_type);

  return true;
}

bool IIOEpollSelector::del(int _fd, int _handler_type)
{
  std::string _handler_type_str;
  if (_handler_type == IHandlerType::IHNDL_READ)
    _handler_type_str = "IHNDL_READ";
  else if (_handler_type == IHandlerType::IHNDL_WRITE)
    _handler_type_str = "IHNDL_WRITE";
  else if (_handler_type == IHandlerType::IHNDL_READ | IHandlerType::IHNDL_WRITE)
    _handler_type_str = "IHNDL_READ | IHNDL_WRITE";

  std::cout << "del(" << _fd << ", " << _handler_type_str << ")" << std::endl;
  struct epoll_event  _ev;
  int                 _ev_type_orig;
  int                 _ev_type;
  int                 _op;

  if (!_M_prepare_epoll_event(&_ev, _fd, _handler_type))
    return false;

  _ev_type_orig = _M_events_type[_fd];
  _ev_type = _ev.events;

  if (_ev_type == _ev_type_orig) {
    _op = EPOLL_CTL_DEL;
  }
  else if ((_ev_type_orig & _ev_type) == _ev_type) {
    _op = EPOLL_CTL_MOD;
    _ev.events = (_ev_type_orig & ~_ev_type);
  }
  else {
    std::cerr << "ERROR epoll del registed type " << _ev_type_orig
      << ", to del type " << _ev_type << std::endl;
    return false;
  }

  _ev.events |= EPOLLET;
  if (epoll_ctl(_M_epoll_fd, _op, _fd, &_ev) < 0) {
    std::cout << "ERROR syscall epoll_ctl in del "
      << strerror(errno) << std::endl;
    return false;
  }

  _M_events_type[_fd] = (_ev_type_orig & ~_ev_type);

  return true;
}

bool IIOEpollSelector::dispatch(const struct timeval *_tvp)
{
  LOG_FUNCTION;
  int       _timeout_in_msec = -1;
  int       _num_ready = 0;
  int       _handler_type = 0;
  int       _fd;
  int       _i;
  struct epoll_event *_evp;

  if (_tvp != NULL) {
    _timeout_in_msec = _tvp->tv_sec * 1000;
    _timeout_in_msec += (_tvp->tv_usec / 1000) + 
                        (_tvp->tv_usec % 1000) / 500;
    if (_timeout_in_msec < 0)
      _timeout_in_msec = -1;
  }

  do {
    _num_ready = epoll_wait(_M_epoll_fd, _M_epoll_events,
                  _M_num_events, _timeout_in_msec);
  } while (_num_ready < 0 && errno == EINTR);

  if (_num_ready < 0 && errno != EINTR) {
    std::cout << "ERROR syscall epoll_wait "
      << strerror(errno) << std::endl;
    return false;
  }

  for (_i = 0; _i < _num_ready; ++_i) {
    _handler_type = 0;
    _evp = _M_epoll_events + _i;
    _fd = _evp->data.fd;
    if (_evp->events & EPOLLIN) {
      _handler_type |= IHandlerType::IHNDL_READ;
      std::cout << "activate_due(" << _fd << ", IN)" << std::endl;
    }
    if (_evp->events & EPOLLOUT) {
      _handler_type |= IHandlerType::IHNDL_WRITE;
      std::cout << "activate_due(" << _fd << ", OUT)" << std::endl;
    }
    if ((_evp->events & EPOLLHUP) || (_evp->events & EPOLLERR)) {
      _handler_type = IHandlerType::IHNDL_READ;
      std::cout << "activate_due(" << _fd << ", HUP || ERR)" << std::endl;
    }
    _M_reactor->activate(_fd, _handler_type);
  }

  return true;
}

IIOEpollSelector::~IIOEpollSelector()
{
  _M_reactor = NULL;

  if (_M_epoll_events != NULL)
    delete _M_epoll_events;
  _M_epoll_events = NULL;

  if (_M_events_type != NULL)
    delete _M_events_type;
  _M_events_type = NULL;

  (void)close(_M_epoll_fd);
  _M_epoll_fd = -1;

  _M_num_events = 0;
}

#endif  // end of HAVE_SYS_EPOLL

