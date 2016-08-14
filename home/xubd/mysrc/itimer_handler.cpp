#include "itimer_handler.h"
#include "iutil.h"

class IEventReactor;
ITimerHandler::ITimerHandler(IEventReactor *pReactor)
{
  m_pEventReactor             =   pReactor;
  //m_tpCookieTLV.usCookieType  =   ITimerCookieType::ICOOKIE_INVALID;
  timerclear(&m_tvExpire);
  m_pScheduleLocation = NULL;
  m_pActivateLocation = NULL;
}

void ITimerHandler::run()
{
  if (m_pExec != NULL) {
    m_pExec->execute((void *)this);
  }
}

ITimerHandler::~ITimerHandler()
{
  m_pEventReactor = NULL;
  m_pExec = NULL;
  timerclear(&m_tvExpire);
}

std::string ITimerHandler::toString() const 
{
  return std::string("ITimer");
}

int ITimerHandler::type() const
{
  return IHandlerType::IHNDL_TIMER;
}

