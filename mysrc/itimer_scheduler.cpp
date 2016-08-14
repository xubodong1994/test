#include "itimer_scheduler.h"
#include <algorithm>
#include <iostream>
#include <cstdio>
#include "iutil.h"

std::string ITimerScheduler::toString() const 
{
  std::string sObjDesc;
  char buf[1024];

  snprintf(buf, sizeof(buf), "{s_nPoolSize = %ld, m_tvInterval = %s, "
                             "m_tvCurSched = %s, m_nCurSchedIdx = %ld,"
                             "m_nMaxActivate = %ld, m_nTotalTimer = %ld}",
                             s_nPoolSize, timer2string(&m_tvInterval).c_str(), 
                             timer2string(&m_tvCurSched).c_str(), m_nCurSchedIdx, 
                             m_nMaxActivate, m_nTotalTimer);

  sObjDesc += buf;

  return sObjDesc;
}

ITimerScheduler::ITimerScheduler(IEventReactor *pReactor, const struct timeval *tvpInterval)
{
  init(pReactor, tvpInterval);
  m_vTimerWheel.resize(s_nPoolSize);
}

void ITimerScheduler::init(IEventReactor *pReactor, const struct timeval *tvpInterval)
{
  if (m_pReactor != pReactor) {
    m_pReactor = pReactor;
  }
  m_nMaxActivate = 256;
  m_nTotalTimer = 0;

  gettimeofday(&m_tvCurSched, NULL);
  m_nCurSchedIdx = 0;

  if (&m_tvInterval != tvpInterval) {
    m_tvInterval = *tvpInterval;
    timer_correct(&m_tvInterval);
    // 定时器最小分辨率100毫秒
    if (timer2usec(&m_tvInterval) < 100000) {
      m_tvInterval.tv_sec = 0;
      m_tvInterval.tv_usec = 100000;
    }
  }
}

bool ITimerScheduler::schedule(ITimerHandler *pTimer, const struct timeval *tvpDelay)
{
  struct timeval          tvSpan, tvExpire, tvCur;
  size_t                  nSchedIdx, n;
  suseconds_t             usecs1, usecs2; 
  ITimerLocation *        pLocation;

  if (pTimer->getScheduleLocation() != NULL) {
    std::cerr << "ERROR duplicate schedule of timer " << std::endl;
    return false;
  }

  if (poolEmpty()) {
    // 如果定时器池为空，最好重新初始化控制变量
    init(m_pReactor, &m_tvInterval);
  }

  if (pTimer == NULL || tvpDelay == NULL) {
    std::cerr << "ERROR schedule a null timer" << std::endl;
    return false; // exception need
  }

  gettimeofday(&tvCur, NULL);
  timeradd(&tvCur, tvpDelay, &tvExpire);

  if (timercmp(&tvCur, &m_tvCurSched, <)) {
    std::cerr << "ERROR exception when schedule timer" << std::endl;
    return false; // exception need
  }

  timersub(&tvExpire, &m_tvCurSched, &tvSpan);
  usecs1 = timer2usec(&tvSpan);
  usecs2 = timer2usec(&m_tvInterval);
  n = (usecs1 + (usecs2 >> 1)) / usecs2;
  nSchedIdx = (n + m_nCurSchedIdx) % s_nPoolSize;

  pLocation = (ITimerLocation *)(pTimer->getScheduleLocation());
  if (pLocation == NULL) {
    pLocation = new ITimerLocation;
    if (pLocation == NULL) {
      std::cerr << "ERROR allocate memory in " << __FUNCTION__ << std::endl;
      return false;
    }
    pTimer->setScheduleLocation(pLocation);
  }

  ITimerHandlerList &lst = m_vTimerWheel[nSchedIdx];
  lst.push_back(pTimer);

  pLocation->nHandlerListIndex = nSchedIdx;
  pLocation->tpHandlerListIter = --lst.end();
  pTimer->setExpireTime(&tvExpire);

  ++m_nTotalTimer;

  return true;
}

bool ITimerScheduler::cancel(ITimerHandler *pTimer)
{
  ITimerHandlerIterator   iter;
  size_t                  nHandlerListIndex;
  ITimerLocation *        pLocation;
  //ITimerCookieTLV *       pCookieTLV;

  pLocation = (ITimerLocation *)pTimer->getScheduleLocation();
  if (pTimer == NULL || pLocation == NULL) {
    std::cerr << "ERROR unsched a null timer " << std::endl;
    return false;
  }

  nHandlerListIndex = pLocation->nHandlerListIndex;

  if (nHandlerListIndex >= s_nPoolSize) {
    std::cerr << "ERROR timer list index out of range!" << std::endl;
    return false;
  }

  iter = pLocation->tpHandlerListIter;
  if (pTimer != *iter) {
    std::cerr << "ERROR fetal error when cancel timer handler" << std::endl;
    return false;
  }

  ITimerHandlerList &lst = m_vTimerWheel[nHandlerListIndex];
  lst.erase(iter);

  delete pLocation;
  pTimer->setScheduleLocation(NULL);

  --m_nTotalTimer;
  return true;
}

int ITimerScheduler::dispatchSingleList(int nListIndex, int nMaxActivate, 
  const struct timeval *tvpSched)
{
  struct timeval        tvSched;
  int                   nActivated;
  ITimerHandler *       pTimer;
  ITimerHandlerIterator iter;

  ITimerHandlerList &lst = m_vTimerWheel[nListIndex];
  nActivated = 0;

  size_t usecs = 0;
  if (m_tvInterval.tv_sec & 1) {
    usecs = 500000;
  }

  tvSched.tv_sec = m_tvInterval.tv_sec >> 1;
  tvSched.tv_usec = (m_tvInterval.tv_usec >> 1) + usecs;
  timeradd(&tvSched, tvpSched, &tvSched);

  iter = lst.begin();
  while (nActivated < nMaxActivate && iter != lst.end()) {
    pTimer = *iter;
    if (timercmp(pTimer->expireTime(), &tvSched, <=)) {
      (void)m_pReactor->activate(pTimer);
      delete (ITimerLocation *)(pTimer->getScheduleLocation());
      pTimer->setScheduleLocation(NULL);
      lst.erase(iter);
      --m_nTotalTimer;
      ++nActivated;
    }
    ++iter;
  }

  return nActivated;
}

bool ITimerScheduler::dispatch(const struct timeval *tvp)
{
  size_t                  idx, nActivated, nMaxActivate;
  ITimerHandlerIterator   iter;
  ITimerHandler *                pTimer;
  struct timeval          tvCur, tvSched, tvExpire;

  if (poolEmpty()) {
    std::cerr << "ERROR ITimerScheduler::dispatch poolEmpty()" << std::endl;
    return false;
  }

  gettimeofday(&tvCur, NULL);
  tvSched = m_tvCurSched;

  if (timercmp(&tvCur, &tvSched, <)) {
    std::cerr << "ERROR fatal error in ITimerScheduler::dispatch() " << std::endl;
    return false; // exception need
  }

  nActivated = 0;
  idx = m_nCurSchedIdx;
  nMaxActivate = std::min(m_nMaxActivate, m_nTotalTimer);

  while (nMaxActivate > 0 && timercmp(&tvSched, &tvCur, <=)) {
    nActivated = dispatchSingleList(idx, nMaxActivate, &tvSched);
    nMaxActivate -= nActivated;
    if (++idx >= s_nPoolSize)
      idx = 0;
    timeradd(&tvSched, &m_tvInterval, &tvSched);
  }

  if (timercmp(&tvSched, &tvCur, >)) {
    timersub(&tvSched, &m_tvInterval, &m_tvCurSched);
    m_nCurSchedIdx = (idx + s_nPoolSize - 1) % s_nPoolSize;
  }
  else {
    m_tvCurSched = tvSched;
    m_nCurSchedIdx = idx;
  }

  return true;
}

void ITimerScheduler::reschedule(ITimerHandler *pTimer)
{
  struct timeval      tvExpire, tvInterval;

  if (pTimer->type() != IHandlerType::IHNDL_PERIODIC)
    return ;
}

void ITimerScheduler::timeoutNext(struct timeval *tvInterval) const
{
  if (tvInterval == NULL)
    ; // exception need

  timerclear(tvInterval);

  if (poolEmpty()) {
    tvInterval->tv_sec = 11111111;
  }
  if (m_tvInterval.tv_sec > 0)
    tvInterval->tv_usec = 500000;
  else
    tvInterval->tv_usec = m_tvInterval.tv_usec >> 1;
}

ITimerScheduler::~ITimerScheduler()
{
  m_vTimerWheel.clear();
  m_pReactor = NULL;
  m_nTotalTimer = 0;
  m_nCurSchedIdx = 0;
  m_tvCurSched.tv_sec = 0;
  m_tvCurSched.tv_usec = 0;
  m_nMaxActivate = 0;
  m_tvInterval.tv_sec = 0;
  m_tvInterval.tv_usec = 0;
}

