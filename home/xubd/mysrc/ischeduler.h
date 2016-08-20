#ifndef __ISCHEDULER_H
#define __ISCHEDULER_H

#include "iobject.h"

struct timeval;

// 事件调度类接口，负责事件派发，抽象基类
class IScheduler : public IObject {
public:
  virtual bool dispatch(const struct timeval *) = 0;
  virtual ~IScheduler() { }
};

#endif  // end __ISCHEDULER_H

