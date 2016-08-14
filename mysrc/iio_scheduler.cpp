#include "iio_scheduler.h"
#include <cstring>
#include <iostream>
#include "ievent_reactor.h"
#include "iio_sel_selector.h"
#ifdef HAVE_SYS_EPOLL
#include "iio_epoll_selector.h"
#endif

IIOScheduler::IIOScheduler(IEventReactor *pReactor)
{
#ifdef HAVE_SYS_EPOLL
  m_pSelector = new IIOEpollSelector(pReactor);
#else
  m_pSelector = new IIOSelSelector(pReactor);
#endif
  memset(m_vIIOHandlers, 0, sizeof(m_vIIOHandlers));
  m_pReactor = pReactor;
  m_nNumIOHandlers = 0;
}

std::string IIOScheduler::toString() const
{
  return std::string("IIOScheduler");
}

bool IIOScheduler::schedule(IReadHandler *pReadHandler)
{
  int fd;
  if (pReadHandler == NULL) {
    std::cerr << "ERROR schedule null read handler " << std::endl;
    return false;
  }

  if (pReadHandler->getScheduleLocation() != NULL) {
    std::cerr << "ERROR duplicate schedule of read handler " << std::endl;
    return false;
  }

  fd = pReadHandler->fd();
  if (fd < 0 || fd >= MAX_FILES) {
    std::cerr << "ERROR fd "<< fd << " invalid" << std::endl;
    return false;
  }

  IIOHandler &h = m_vIIOHandlers[fd];
  if (h.pReadHandler != NULL && h.pReadHandler != pReadHandler) {
    std::cerr << "ERROR too many read handlers for fd " << fd << std::endl;
    return false;
  }

  if(m_pSelector->add(fd, pReadHandler->type())) {
    h.pReadHandler = pReadHandler;
    ++m_nNumIOHandlers;
    return true;
  }

  return false;
}

bool IIOScheduler::schedule(IWriteHandler *pWriteHandler)
{
  int fd;
  if (pWriteHandler == NULL) {
    std::cerr << "ERROR schedule null write handler " << std::endl;
    return false;
  }

  if (pWriteHandler->getScheduleLocation() != NULL) {
    std::cerr << "ERROR duplicate schedule of write handler " << std::endl;
    return false;
  }

  fd = pWriteHandler->fd();
  if (fd < 0 || fd >= MAX_FILES) {
    std::cerr << "ERROR fd " << fd << " invalid" << std::endl;
    return false;
  }

  IIOHandler &h = m_vIIOHandlers[fd];
  if (h.pWriteHandler != NULL && h.pWriteHandler != pWriteHandler) {
    std::cerr << "ERROR too many write handlers for fd " << fd << std::endl;
    return false;
  }

  if (m_pSelector->add(fd, pWriteHandler->type()) == true) {
    h.pWriteHandler = pWriteHandler;
    ++m_nNumIOHandlers;
    return true;
  }
  return false;
}

bool IIOScheduler::cancel(IReadHandler *pReadHandler)
{
  int fd;
  if (pReadHandler == NULL) {
    std::cerr << "ERROR schedule null read handler " << std::endl;
    return false;
  }

  fd = pReadHandler->fd();
  if (fd < 0 || fd >= MAX_FILES) {
    std::cerr << "ERROR fd " << fd << " invalid" << std::endl;
    return false;
  }

  IIOHandler &h = m_vIIOHandlers[fd];
  if (h.pReadHandler == NULL || h.pReadHandler != pReadHandler) {
    std::cerr << "ERROR cancel read handler failed ! " << std::endl;
    return false;
  }

  if (m_pSelector->del(fd, pReadHandler->type()) == true) {
    h.pReadHandler = NULL;
    --m_nNumIOHandlers;
    return true;
  }

  return false;
}

bool IIOScheduler::cancel(int fd, int handlerType)
{
  IIOHandler *h = NULL;
  if (fd < 0 || fd >= MAX_FILES) {
    std::cerr << "ERROR fd " << fd << " invalid " << std::endl;
    return false;
  }

  h = &m_vIIOHandlers[fd];

  if (h->pReadHandler == NULL && (handlerType & IHandlerType::IHNDL_READ)) {
    handlerType = (handlerType & ~IHandlerType::IHNDL_READ);
  }

  if (h->pWriteHandler == NULL && (handlerType & IHandlerType::IHNDL_WRITE)) {
    handlerType = (handlerType & ~IHandlerType::IHNDL_WRITE);
  }

  std::cout << "IIOSelSelector::del(" << fd << ", " << handlerType << ")" << std::endl;
  if (m_pSelector->del(fd, handlerType) == true) {
    --m_nNumIOHandlers;
    if (handlerType & IHandlerType::IHNDL_READ)
      h->pReadHandler = NULL;
    if (handlerType & IHandlerType::IHNDL_WRITE)
      h->pWriteHandler = NULL;
    return true;
  }

  return false;
}

bool IIOScheduler::cancel(IWriteHandler *pWriteHandler)
{
  int fd;
  if (pWriteHandler == NULL) {
    std::cerr << "ERROR schedule null write handler " << std::endl;
    return false;
  }

  fd = pWriteHandler->fd();
  if (fd < 0 || fd >= MAX_FILES) {
    std::cerr << "ERROR fd " << fd << " invalid" << std::endl;
    return false;
  }

  IIOHandler &h = m_vIIOHandlers[fd];
  if (h.pWriteHandler == NULL || h.pWriteHandler != pWriteHandler) {
    std::cerr << "ERROR cancel write handler failed ! " << std::endl;
    return false;
  }

  if (m_pSelector->del(fd, pWriteHandler->type()) == true) {
    h.pWriteHandler = NULL;
    --m_nNumIOHandlers;
    return true;
  }

  return false;
}

bool IIOScheduler::dispatch(const struct timeval *tvp)
{
  return m_pSelector->dispatch(tvp);
}

void IIOScheduler::activate(int fd, int handlerType)
{
  std::string _handler_type_str;
  if (handlerType == IHandlerType::IHNDL_READ)
    _handler_type_str = "IHNDL_READ";
  else if (handlerType == IHandlerType::IHNDL_WRITE)
    _handler_type_str = "IHNDL_WRITE";
  else if (handlerType == (IHandlerType::IHNDL_READ | IHandlerType::IHNDL_WRITE))
    _handler_type_str = "IHNDL_READ | IHNDL_WRITE";

  std::cout << "activate(" << fd << ", " << _handler_type_str << ")" << std::endl;
  IEventHandler *pHandler = NULL;
  IIOHandler &h = m_vIIOHandlers[fd];
  switch (handlerType) {
  case IHandlerType::IHNDL_READ:
    m_pReactor->activate(h.pReadHandler);
    break;
  case IHandlerType::IHNDL_WRITE:
    m_pReactor->activate(h.pWriteHandler);
    break;
  case IHandlerType::IHNDL_READ | IHandlerType::IHNDL_WRITE:
    m_pReactor->activate(h.pReadHandler);
    m_pReactor->activate(h.pWriteHandler);
    break;
  }
}

IIOScheduler::~IIOScheduler()
{
  delete m_pSelector;
  m_pSelector = NULL;

  memset(m_vIIOHandlers, 0, sizeof(m_vIIOHandlers));

  m_pReactor = NULL;

  m_nNumIOHandlers = 0;
}

