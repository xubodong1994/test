#ifndef __ITIMER_HANDLER_H
#define __ITIMER_HANDLER_H

#include <vector>
#include <list>
#include "ievent_handler.h"
#include "iutil.h"
#ifdef _HAVE_TIMER_ADD
#include <sys/time.h>
#endif

#ifndef MAX_COOKIE_SIZE
#define MAX_COOKIE_SIZE 32
#endif

class   IEventReactor;
class   IExecutable;

// 定时器接口，抽象基类
class ITimerHandler : public IEventHandler {
protected:
  IEventReactor *         m_pEventReactor;
  struct timeval          m_tvExpire;
  //ITimerCookieTLV         m_tpCookieTLV;

public:
  ITimerHandler(IEventReactor *pReactor);

  virtual void run();

  virtual std::string toString() const;

  virtual int type() const;

  virtual int priority() const {
    return IHandlerType::IHNDL_TIMER;
  }

  const struct timeval *expireTime() const { 
    return &m_tvExpire; 
  }
  void setExpireTime(const struct timeval *tvpExpire) { 
    m_tvExpire = *tvpExpire; 
  }

  virtual ~ITimerHandler();
};

#endif  // end __ITIMER_HANDLER_H

