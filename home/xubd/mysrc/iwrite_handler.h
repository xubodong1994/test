#ifndef __IWRITE_HANDLER_H
#define __IWRITE_HANDLER_H
#include "ievent_handler.h"

class IWriteHandler : public IEventHandler {
private:
  int m_nFd;

public:
  IWriteHandler(int fd, IEventReactor *pReactor);
  int fd() { return m_nFd; }

public:
  virtual std::string toString() const;
  virtual int priority() const;
  virtual int type() const;
  virtual void run();
  virtual ~IWriteHandler() { }
};

#endif  // end __IWRITE_HANDLER_H

