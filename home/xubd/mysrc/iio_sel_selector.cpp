#include "iio_sel_selector.h"
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <errno.h>

extern int errno;

IIOSelSelector::IIOSelSelector(IEventReactor *pReactor)
{
  m_nfds = 0;
  FD_ZERO(&m_tpReadSet);
  FD_ZERO(&m_tpWriteSet);
  FD_ZERO(&m_tpInOutReadSet);
  FD_ZERO(&m_tpInOutWriteSet);
  FD_ZERO(&m_tpInOutErrorSet);
  m_pReactor = pReactor;
}

std::string IIOSelSelector::toString() const 
{
  int         _fd_set;
  std::string _read_str, _write_str, _obj_str;
  char        _int_buf[6];

  _read_str = "read set = { ";
  _write_str = "write set = { ";

  for (int _i = 0; _i < 1024; ++_i) {
    if (FD_ISSET(_i, &m_tpReadSet)) {
      sprintf(_int_buf, "%d ", _i);
      _read_str += _int_buf;
    }
    if (FD_ISSET(_i, &m_tpWriteSet)) {
      sprintf(_int_buf, "%d ", _i);
      _write_str += _int_buf;
    }
  }

  _obj_str = _read_str + "} " + _write_str + "}";
  return _obj_str;
}

static std::string fdset_to_string(fd_set &_set)
{
  std::string _obj_str;
  char _buf[6];

  _obj_str = "{ ";
  for (int _i = 0; _i < 1024; ++_i) {
    if (FD_ISSET(_i, &_set)) {
      sprintf(_buf, "%d ", _i);
      _obj_str += _buf;
    }
  }
  _obj_str += "}";

  return _obj_str;
}

bool IIOSelSelector::add(int _fd, int _handler_type)
{
  int  _handler_type_all = IHandlerType::IHNDL_READ | IHandlerType::IHNDL_WRITE;

  if ((_handler_type & _handler_type_all) == 0) {
    std::cerr << "ERROR IIOSelSelector add type unknown" << std::endl;
    return false;
  }

  if (_handler_type & IHandlerType::IHNDL_READ)
    addRead(_fd);

  if (_handler_type & IHandlerType::IHNDL_WRITE)
    addWrite(_fd);

  return true;
}

bool IIOSelSelector::del(int _fd, int _handler_type)
{
  int  _handler_type_all = IHandlerType::IHNDL_READ | IHandlerType::IHNDL_WRITE;

  if ((_handler_type & _handler_type_all) == 0) {
    std::cerr << "ERROR IIOSelSelector del type unknown" << std::endl;
    return false;
  }

  if (_handler_type & IHandlerType::IHNDL_READ)
    delRead(_fd);

  if (_handler_type & IHandlerType::IHNDL_WRITE)
    delWrite(_fd);

  std::cout << "IIOSelSelector::del " << toString() << std::endl;
  return true;
}

bool IIOSelSelector::dispatch(const struct timeval *tvp)
{
  int             i = 0;
  int             j = 0;
  int             r = 0;
  int             type = 0;
  struct timeval *tvp1 = const_cast<struct timeval *>(tvp);

  FD_COPY(&m_tpReadSet, &m_tpInOutReadSet);
  FD_COPY(&m_tpWriteSet, &m_tpInOutWriteSet);
  FD_ZERO(&m_tpInOutErrorSet);

  std::cout << "IIOSelSelector: " << toString() << " _in_out_read " << fdset_to_string(m_tpInOutReadSet)
    << " _in_out_write " << fdset_to_string(m_tpInOutWriteSet) << " _in_out_error " 
    << fdset_to_string(m_tpInOutErrorSet) << std::endl;
  r = select(m_nfds, &m_tpInOutReadSet, &m_tpInOutWriteSet, &m_tpInOutErrorSet, tvp1);
  if (r < 0) {
    std::cerr << "ERROR system call select failed: " << strerror(errno) << std::endl;
    return false;
  }
  std::cout << "IIOSelSelector: " << toString() << " _in_out_read " << fdset_to_string(m_tpInOutReadSet)
    << " _in_out_write " << fdset_to_string(m_tpInOutWriteSet) << " _in_out_error " 
    << fdset_to_string(m_tpInOutErrorSet) << std::endl;

  j = rand() % m_nfds;
  for (i = 0; i < m_nfds; ++i) {
    type = 0;
    if (FD_ISSET(j, &m_tpInOutErrorSet))
      type |= (IHandlerType::IHNDL_READ | IHandlerType::IHNDL_WRITE);
    if (FD_ISSET(j, &m_tpInOutReadSet))
      type |= IHandlerType::IHNDL_READ;
    if (FD_ISSET(j, &m_tpInOutWriteSet))
      type |= IHandlerType::IHNDL_WRITE;

    if (type != 0) {
      std::cout << "fd: " << j << " type: " << 
        ((type == IHandlerType::IHNDL_READ) ? "read" : "write") << std::endl;
      m_pReactor->activate(j, type);
    }

    if (++j == m_nfds)
      j = 0;
  }
  return true;
}

bool IIOSelSelector::addRead(int fd)
{
  bool bRet = true;
  std::cout << "IIOSelSelector::addRead(" << fd << ")" << toString() << std::endl;
  if (fd < 0 || FD_ISSET(fd, &m_tpReadSet)) {
    bRet = false;
  } else if (fd >= m_nfds) {
    m_nfds = fd + 1;
  }
  FD_SET(fd, &m_tpReadSet);
  std::cout << "IIOSelSelector::addRead(" << fd << ")" << toString() << std::endl;
  return bRet;
}

bool IIOSelSelector::addWrite(int fd)
{
  bool bRet = true;
  std::cout << "IIOSelSelector::addWrite(" << fd << ")" << toString() << std::endl;
  if (fd < 0 || FD_ISSET(fd, &m_tpWriteSet)) {
    bRet = false;
  }
  else if (fd >= m_nfds) {
    m_nfds = fd + 1;
  }
  FD_SET(fd, &m_tpWriteSet);
  std::cout << "IIOSelSelector::addWrite(" << fd << ")" << toString() << std::endl;
  return bRet;
}

bool IIOSelSelector::delRead(int fd)
{
  bool bRet = true;
  if (fd >= m_nfds || fd < 0 || !FD_ISSET(fd, &m_tpReadSet)) {
    bRet = false;
  }
  else {
    if (fd == m_nfds - 1 && !FD_ISSET(fd, &m_tpWriteSet)) {
      // stub
    }
    FD_CLR(fd, &m_tpReadSet);
  }
  return bRet;
}

bool IIOSelSelector::delWrite(int fd)
{
  bool bRet = true;
  if (fd >= m_nfds || fd < 0 || !FD_ISSET(fd, &m_tpWriteSet)) {
    bRet = false;
  }
  else {
    if (fd == m_nfds - 1 && !FD_ISSET(fd, &m_tpReadSet)) {
      // stub
    }
    FD_CLR(fd, &m_tpWriteSet);
  }
  return bRet;
}

