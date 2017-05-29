#ifndef ICOMMANDABLE_HPP
#define ICOMMANDABLE_HPP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "server/Command.hpp"

class ICommandable
{
  public:
    virtual const char* processCmd(Command *_C) = 0;
};

#endif
