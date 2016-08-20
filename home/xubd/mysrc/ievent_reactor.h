#ifndef __IEVENT_REACTOR_H
#define __IEVENT_REACTOR_H

#include "ievent_handler.h"
#include "itimer_handler.h"
#include "iio_scheduler.h"
#include <list>
class IScheduler;
class ITimerScheduler;

typedef std::list<IEventHandler *>  IHandlerList;

class IEventReactor {
  typedef struct IActivateLocation {
    IHandlerList::iterator iter;
  }ILocation;
private:
  IHandlerList        m_lstActiveHandler;
  ITimerScheduler *   m_pTimerScheduler;
  IIOScheduler *      m_pIOScheduler;
  size_t              m_nActivatedHandlers;

public:
  IEventReactor();

  void runEventLoop();

  void activate(IEventHandler *pHandler);
  void activate(int fd, int handlerType);

  void schedule(ITimerHandler *pTimer, const struct timeval *tvpDelay, void *_arg);
  void schedule(IReadHandler *pReadHandler, void *_arg);
  void schedule(IWriteHandler *pWriteHandler, void *_arg);

  void cancel(IEventHandler *pHandler);
  void cancel(int fd, int handlerType);
private:
  bool haveActiveHandlers() const;
};

#endif  // end __IEVENT_REACTOR_H

