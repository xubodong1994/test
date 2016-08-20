#include "iwrite_handler.h"

IWriteHandler::IWriteHandler(int fd, IEventReactor *pReactor)
{
  m_nFd = fd;
  m_pReactor = pReactor;
  m_pExec = NULL;
  m_pScheduleLocation = NULL;
  m_pActivateLocation = NULL;
}

std::string IWriteHandler::toString() const
{
  return std::string("IWriteHandler");
}

int IWriteHandler::priority() const
{
  return IHandlerPriority::IHNDL_PRIOR_HIGH;
}

int IWriteHandler::type() const
{
  return IHandlerType::IHNDL_WRITE;
}

void IWriteHandler::run()
{
  if (m_pExec != NULL) {
    m_pExec->execute(this);
  }
}

