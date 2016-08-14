#ifndef __IRUNNABLE_H
#define __IRUNNABLE_H

#include "iobject.h"

class IExecutable : public IObject {
public:
  virtual std::string toString() const {
    return std::string("IExecutable");
  }
  virtual void execute(void *pArg) = 0;
  virtual ~IExecutable() { }
};

#endif  // end __IRUNNABLE_H

