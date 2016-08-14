#ifndef __IOBJECT_H
#define __IOBJECT_H

#include <string>

class IObject {
public:
  virtual std::string toString() const = 0;
  virtual ~IObject() { }
};

#endif  // end __IOBJECT_H

