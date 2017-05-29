#ifndef IGIMBLE_HPP
#define IGIMBLE_HPP

#include <stdlib.h>
#include <stdio.h>

class IGimble
{
  public:
    virtual ~IGimble() {;};
    virtual bool isAttached() = 0;
    virtual bool setPan(double deg) = 0;
    virtual bool setTilt(double deg) = 0;
    virtual bool stepPan(double deg) = 0;
    virtual bool stepTilt(double deg) = 0;
    virtual double getPan() = 0;
    virtual double getTilt() = 0;
  
};

#endif
