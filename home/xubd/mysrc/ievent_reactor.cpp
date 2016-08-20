#include "ievent_reactor.h"
#include "itimer_scheduler.h"
#include "iutil.h"
#include <unistd.h>
#include <iostream>

using namespace std;

IEventReactor::IEventReactor()
{
  struct timeval tvInterval;
  tvInterval.tv_sec = 0;
  tvInterval.tv_usec = 100000;

  m_nActivatedHandlers = 0;
  m_pTimerScheduler = new ITimerScheduler(this, &tvInterval);
  m_pIOScheduler = new IIOScheduler(this);
}

bool IEventReactor::haveActiveHandlers() const
{
  return m_nActivatedHandlers > 0;
}

void IEventReactor::runEventLoop()
{
  bool                    bRunning = true;
  IHandlerList::iterator  iter;
  IEventHandler *         pHandler;
  struct timeval          tvNext;
  ILocation *             pLocation;

  while (bRunning) {
    m_pTimerScheduler->timeoutNext(&tvNext);
    std::cout << "time_next " << timer2string(&tvNext) << std::endl;

    if (!m_pIOScheduler->poolEmpty()) {
      std::cout << "IEventReactor::runEventLoop() before io dispatch" << std::endl;
      m_pIOScheduler->dispatch(&tvNext);
      std::cout << "IEventReactor::runEventLoop() after io dispatch" << std::endl;
    }

    if (!m_pTimerScheduler->poolEmpty()) {
      std::cout << "IEventReactor::runEventLoop() before timer dispatch" << std::endl;
      m_pTimerScheduler->dispatch(NULL);
      std::cout << "IEventReactor::runEventLoop() after timer dispatch" << std::endl;
    }

    iter = m_lstActiveHandler.begin();
    std::cout << "active size: " << m_lstActiveHandler.size() << std::endl;
    while (iter != m_lstActiveHandler.end()) {
      pHandler = (*iter);
      //std::cout << pHandler->toString() << std::endl;
      pLocation = static_cast<ILocation *>((*iter)->getActivateLocation());
      if (pLocation != NULL) {
        delete pLocation;
        (*iter)->setActivateLocation(NULL);
      }
      else {
        std::cout << "handler activated location is null " << std::endl;
      }
      (*iter)->run();
      m_lstActiveHandler.pop_front();
      --m_nActivatedHandlers;
      iter = m_lstActiveHandler.begin();
    }

    if (m_pTimerScheduler->poolEmpty() && !haveActiveHandlers() && m_pIOScheduler->poolEmpty())
      bRunning = false;
  }
}

void IEventReactor::activate(IEventHandler *pHandler)
{
  ILocation *pLocation = NULL;
  if (pHandler == NULL) {
    std::cerr << "ERROR activate an null handler " << std::endl;
    return ;
  }
  m_lstActiveHandler.push_back(pHandler);
  pLocation = new ILocation;
  pLocation->iter = --m_lstActiveHandler.end();
  pHandler->setActivateLocation(pLocation);
  ++m_nActivatedHandlers;
}

void IEventReactor::activate(int fd, int handlerType)
{
  m_pIOScheduler->activate(fd, handlerType);
}

void IEventReactor::schedule(ITimerHandler *pTimer, const struct timeval *tvpDelay, void *_arg)
{
  if (pTimer != NULL) {
    pTimer->argument(_arg);
    m_pTimerScheduler->schedule(pTimer, tvpDelay);
  }
  else {
    std::cerr << "ERROR schedule null timer" << std::endl;
  }
}

void IEventReactor::schedule(IReadHandler *pReadHandler, void *_arg)
{
  if (pReadHandler != NULL) {
    pReadHandler->argument(_arg);
    m_pIOScheduler->schedule(pReadHandler);
  }
  else {
    std::cerr << "ERROR schedule null read handler" << std::endl;
  }
}

void IEventReactor::schedule(IWriteHandler *pWriteHandler, void *_arg)
{
  if (pWriteHandler != NULL) {
    pWriteHandler->argument(_arg);
    m_pIOScheduler->schedule(pWriteHandler);
  }
  else {
    std::cerr << "ERROR schedule null write handler" << std::endl;
  }
}

void IEventReactor::cancel(IEventHandler *pHandler)
{
  std::cout << "IEventReactor::cancel " << std::endl;
  ILocation *pLocation = NULL;
  if (pHandler == NULL) {
    std::cerr << "ERROR cancel null event handler" << std::endl;
    return ;
  }
  pLocation = (ILocation *)(pHandler->getActivateLocation());
  if (pLocation != NULL) {
    m_lstActiveHandler.erase(pLocation->iter);
    --m_nActivatedHandlers;
    delete pLocation;
    pHandler->setActivateLocation(NULL);
  }
  /*
  else {
    std::cerr << "ERROR handler activated location is null " << std::endl;
    return ;
  }
  */

  switch(pHandler->type()) {
  case IHandlerType::IHNDL_TIMER:
    m_pTimerScheduler->cancel(dynamic_cast<ITimerHandler *>(pHandler));
    break;
  case IHandlerType::IHNDL_READ:
    std::cout << "cancel read handler fd " << ((IReadHandler *)pHandler)->fd() << std::endl;
    m_pIOScheduler->cancel(dynamic_cast<IReadHandler *>(pHandler));
    break;
  case IHandlerType::IHNDL_WRITE:
    std::cout << "cancel write handler fd " << ((IWriteHandler *)pHandler)->fd() << std::endl;
    m_pIOScheduler->cancel(dynamic_cast<IWriteHandler *>(pHandler));
    break;
  default:
    std::cerr << "ERROR cancel handler have an unkown type " << std::endl;
    break;
  }
  std::cout << "IEventReactor::cancel end " << std::endl;
  return ;
}

void IEventReactor::cancel(int fd, int handlerType)
{
  m_pIOScheduler->cancel(fd, handlerType);
}

