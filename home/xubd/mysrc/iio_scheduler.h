#ifndef __IIO_SCHEDULER_H
#define __IIO_SCHEDULER_H
#include "ischeduler.h"
#include "iread_handler.h"
#include "iwrite_handler.h"
#include "iio_selector.h"

#if defined(MAX_FILES)
#undef MAX_FILES
#endif
#define MAX_FILES 65535

class IEventReactor;

class IIOScheduler : public IScheduler {
  struct IIOHandler {
    IReadHandler *  pReadHandler;
    IWriteHandler * pWriteHandler;
  };

private:
  IIOSelector *     m_pSelector;
  IIOHandler        m_vIIOHandlers[MAX_FILES];//句柄的映射
  IEventReactor *   m_pReactor;
  size_t            m_nNumIOHandlers;
  
public:  
  bool schedule(IReadHandler *pReadHandler);
  bool schedule(IWriteHandler *pWriteHandler);
  bool cancel(IReadHandler *pReadHandler);
  bool cancel(IWriteHandler *pWriteHandler);
  bool cancel(int fd, int handlerType);
  bool dispatch(const struct timeval *tvp);

  void activate(int fd, int handlerType);

  bool poolEmpty() const { return m_nNumIOHandlers == 0; }
public:
  IIOScheduler(IEventReactor *pReactor);

public:
  virtual std::string toString() const;
  virtual ~IIOScheduler();
};

#endif  // end __IIO_SCHEDULER_H

