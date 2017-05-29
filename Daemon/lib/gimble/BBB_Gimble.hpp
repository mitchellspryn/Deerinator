#ifndef BBBGIMBLE_HPP
#define BBBGIMBLE_HPP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "server/Command.hpp"
#include "interface/IGimble.hpp"
#include "interface/ICommandable.hpp"

#define WORKSPACE_SIZE 64

#define STARTUP_PAN  90.
#define STARTUP_TILT 25.

class BBBGimble : public IGimble, public ICommandable
{
  public:
    BBBGimble(int panServoChannel);
    ~BBBGimble();
    bool isAttached();
    bool setPan(double deg);
    bool setTilt(double deg);
    bool stepPan(double deg);
    bool stepTilt(double deg);
    double getPan();
    double getTilt();
    const char* processCmd(Command* _C);

  private:
    FILE* initServo(int channel);
    FILE* panServoFP;
    FILE* tiltServoFP;
    bool Attached;
    double panDuty;
    double tiltDuty;
    bool enabledPWM;
    
    char* workspace;
};

#endif
