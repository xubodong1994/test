#ifndef __IEVENT_HANDLER_H
#define __IEVENT_HANDLER_H

#include "iexecutable.h"

class   IExecutable;
class   IEventReactor;

struct IHandlerType {
  enum {
    IHNDL_NONE = 1 << 0,
    IHNDL_TIMER = 1 << 1,
    IHNDL_READ = 1 << 2,
    IHNDL_WRITE = 1 << 3,
    IHNDL_SIGNAL = 1 << 4,
    IHNDL_PERIODIC = 1 << 5,
    IHNDL_PERSIST = 1 << 6
  };
};

struct IHandlerPriority {
  enum {
    IHNDL_PRIOR_BEGIN = 0,
    IHNDL_PRIOR_HIGH = 0,
    IHNDL_PRIOR_MID,
    IHNDL_PRIOR_LOW,
    IHNDL_PRIOR_COUNT
  };
};

class IEventHandler : public IObject {
public:
  virtual void run() = 0;
  virtual std::string toString() const = 0;
  virtual int priority() const = 0;
  virtual int type() const = 0;
  virtual ~IEventHandler() { }

public:
  //bool  suspended() const { return m_bSuspended; }
  //void  suspend() { m_bSuspended = true; }
  void  setExecute(const IExecutable *pExec) 
    { m_pExec = const_cast<IExecutable *>(pExec); }
  IExecutable *getExecute() const { return m_pExec; }

  IEventReactor *reactor() const { return m_pReactor; }

  void *getScheduleLocation() {
    return m_pScheduleLocation;
  }

  void setScheduleLocation(void *p) {
    m_pScheduleLocation = p;
  }

  void *getActivateLocation() {
    return m_pActivateLocation;
  }

  void setActivateLocation(void *p) {
    m_pActivateLocation = p;
  }

  void *argument() const {
    return _M_argument;
  }

  void argument(void *_arg) {
    _M_argument = _arg;
  }


protected:
  //bool            m_bSuspended;
  IExecutable *   m_pExec;
  IEventReactor * m_pReactor;

  void *          m_pScheduleLocation;
  void *          m_pActivateLocation;
  void *          _M_argument;
};

#endif // end __IEVENT_HANDLER_H

