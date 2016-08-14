#ifndef __ISIGNAL_HANDLER_H
#define __ISIGNAL_HANDLER_H

#include "ievent_handler.h"
class IEventReactor;

class ISignalHandler : public IEventHandler {
public:
  ISignalHandler(int _sig_num, IEventReactor *_reactor);
  virtual void run();
  virtual std::string toString() const;
  virtual int priority() const;
  virtual int type() const;
  virtual ~ISignalHandler() { }

  int signal_number() const;

private:
  int   _M_sig_num;
};

#endif  // end of __ISIGNAL_HANDLER_H

