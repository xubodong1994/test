#ifndef __IREAD_HANDLER_H
#define __IREAD_HANDLER_H
#include "ievent_handler.h"

class IReadHandler : public IEventHandler {
private:
  int   m_nFd;

public:
  IReadHandler(int fd, IEventReactor *pReactor);
  int fd() { return m_nFd; }

public:
  virtual std::string toString() const;
  virtual int priority() const;
  virtual int type() const;
  virtual void run();
  virtual ~IReadHandler() { }
};

#endif  // end __IREAD_HANDLER_H

