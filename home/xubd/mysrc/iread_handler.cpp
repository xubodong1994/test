#include "iread_handler.h"

IReadHandler::IReadHandler(int fd, IEventReactor *pReactor)
{
  m_pReactor = pReactor;
  m_nFd = fd;
  m_pExec = NULL;
  m_pScheduleLocation = NULL;
  m_pActivateLocation = NULL;
}

void IReadHandler::run()
{
  if (m_pExec != NULL) {
    m_pExec->execute(this);
  }
}

std::string IReadHandler::toString() const
{
  return std::string("IReadHandler");
}

int IReadHandler::priority() const
{
  return IHandlerPriority::IHNDL_PRIOR_HIGH;
}

int IReadHandler::type() const
{
  return IHandlerType::IHNDL_READ;
}

