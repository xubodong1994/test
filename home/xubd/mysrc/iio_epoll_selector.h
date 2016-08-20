#ifdef HAVE_SYS_EPOLL
#ifndef __IIO_EPOLL_SELECTOR_H
#define __IIO_EPOLL_SELECTOR_H
#include <sys/epoll.h>
#include <string>
#include "iio_selector.h"

class IEventReactor;

class IIOEpollSelector : public IIOSelector {
public:
  IIOEpollSelector(IEventReactor *_reactor);
  virtual std::string toString() const;
  virtual bool add(int _fd, int _handler_type);
  virtual bool del(int _fd, int _handler_type);
  virtual bool dispatch(const struct timeval *_tvp);
  virtual ~IIOEpollSelector();

private:
  IEventReactor *       _M_reactor;
  struct epoll_event *  _M_epoll_events;
  size_t                _M_num_events;
  int *                 _M_events_type;
  int                   _M_epoll_fd;
  bool _M_prepare_epoll_event(struct epoll_event *_ev, int _fd, int _handler_type);
};

#endif	// end of __IIO_EPOLL_SELECTOR_H

#endif  // end of HAVE_SYS_EPOLL

