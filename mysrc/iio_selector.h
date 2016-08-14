#ifndef __IIO_SELECTOR_H
#define __IIO_SELECTOR_H
#include "iobject.h"

class IIOSelector : public IObject {
public:
  virtual std::string toString() const = 0;
  virtual bool add(int fd, int handlerType) = 0;
  virtual bool del(int fd, int handlerType) = 0;
  virtual bool dispatch(const struct timeval *tvp) = 0;
  virtual ~IIOSelector() { }
};

#endif

