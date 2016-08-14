#include "isignal_handler.h"
#include "ievent_reactor.h"
#include <iostream>

ISignalHandler::ISignalHandler(int _sig_num, IEventReactor *_reactor)
{
  _M_sig_num = _sig_num;
  m_pReactor = _reactor;
}

void ISignalHandler::run()
{
  if (m_pExec != NULL)
    m_pExec->execute(this);
}

std::string ISignalHandler::toString() const
{
  return std::string("ISignalHandler");
}

int ISignalHandler::priority() const
{
  return IHandlerPriority::IHNDL_PRIOR_HIGH;
}

int ISignalHandler::type() const
{
  return IHandlerType::IHNDL_SIGNAL;
}

int ISignalHandler::signal_number() const
{
  return _M_sig_num;
}

