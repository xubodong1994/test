#ifndef __IIO_SEL_SELECTOR_H
#define __IIO_SEL_SELECTOR_H
#include <sys/select.h>
#include "ievent_reactor.h"
#include "iio_selector.h"

class IIOSelSelector : public IIOSelector {
private:
  int             m_nfds;
  fd_set          m_tpReadSet;
  fd_set          m_tpWriteSet;
  fd_set          m_tpInOutReadSet;
  fd_set          m_tpInOutWriteSet;
  fd_set          m_tpInOutErrorSet;
  IEventReactor * m_pReactor;

public:
  virtual std::string toString() const;
  virtual bool add(int fd, int handlerType);
  virtual bool del(int fd, int handlerType);
  virtual bool dispatch(const struct timeval *tvp);

public:
  IIOSelSelector(IEventReactor *pReactor);
  virtual ~IIOSelSelector() { }

private:
  bool addRead(int fd);
  bool addWrite(int fd);
  bool delRead(int fd);
  bool delWrite(int fd);
};

#endif  // end __IIO_SEL_SELECTOR_H

