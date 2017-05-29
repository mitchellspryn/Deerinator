#ifndef TOYACCEL_HPP
#define TOYACCEL_HPP

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <server/Command.hpp>

#define FIRE_TIME 1000
#define FIRE_DUTY_TIME 100

class toyAccel
{
  private:
    bool attached;
    FILE* fireFP;
  public:
    toyAccel();

    bool isAttached();
    void fire(int ms);
    const char* processCmd(Command* _C);
};

#endif
