#ifndef IACCELERATOR_HPP
#define IACCELERATOR_HPP

#include <stdlib.h>
#include <stdio.h>

class IAccelerator
{
  public:
    virtual void Initialize() = 0;
    virtual bool isAttached() = 0;
    virtual bool fire() = 0;
};
#endif
