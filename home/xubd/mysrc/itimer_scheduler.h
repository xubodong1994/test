#ifndef __ITIMER_SCHEDULER_H
#define __ITIMER_SCHEDULER_H

#include "ischeduler.h"
#include "ievent_reactor.h"
#include "itimer_handler.h"

struct greater : public std::binary_function<ITimerHandler *, ITimerHandler *, bool>
{
  bool operator()(const ITimerHandler *&x, const ITimerHandler *&y) {
    return timercmp(x->expireTime(), y->expireTime(), >);
  }
};

// 定时器调度器，管理所有的定时器
class ITimerScheduler : public IScheduler {
  typedef std::list<ITimerHandler *>  ITimerHandlerList;
  typedef ITimerHandlerList::iterator ITimerHandlerIterator;
  typedef struct ITimerScheduledLocation {
    size_t                nHandlerListIndex;
    ITimerHandlerIterator tpHandlerListIter;
  }ITimerLocation;
private:
  std::vector<ITimerHandlerList>      m_vTimerWheel;       
  std::vector<ITimerHandler *>        m_vTimerQueue;
  static const unsigned long          s_nPoolSize = 1024;
  struct timeval                      m_tvInterval; // 定时器分辨率，以毫秒为单位，最小100毫秒
  struct timeval                      m_tvCurSched;
  size_t                              m_nCurSchedIdx;
  size_t                              m_nMaxActivate;
  size_t                              m_nTotalTimer;
  IEventReactor *                     m_pReactor;
  
public:
  ITimerScheduler(IEventReactor *pReactor, const struct timeval *tvpInterval);
  bool schedule(ITimerHandler *pTimer, const struct timeval *tvpDelay);
  bool cancel(ITimerHandler *pTimer);
  bool poolEmpty() const { return m_nTotalTimer == 0; }
  size_t poolSize() const { return m_nTotalTimer; }
  void timeoutNext(struct timeval *tvInterval) const;
  
public:
  virtual ~ITimerScheduler();
  virtual bool dispatch(const struct timeval *tvp);
  virtual std::string toString() const;

private:
  void reschedule(ITimerHandler *pTimer);
  void init(IEventReactor *pReactor, const struct timeval *tvpInterval);
  int dispatchSingleList(int nListIndex, int nMaxActivate, const struct timeval *tvpSched);
  ITimerHandler *pop(const struct timeval *tvpExpire);
};

#endif  // end __ITIMER_SCHEDULER_H

