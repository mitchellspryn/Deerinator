#include "BBB_Gimble.hpp"

BBBGimble::BBBGimble(int channel)
{
  this->Attached = false;
  this->enabledPWM = false;

  if (channel == 1)
  {
    this->panServoFP = this->initServo(1);
    this->tiltServoFP = this->initServo(2);
  }
  else
  {
    this->panServoFP  = this->initServo(2);
    this->tiltServoFP = this->initServo(1);
  }

  /*TODO: initialize*/
  if ( (this->panServoFP == NULL) || (this->tiltServoFP == NULL) )
  {
    fprintf(stdout, "ERROR: Couldn't attach servos.\n");
    return;
  }

  /*TODO: Determine the actual position in which these need to be set on startup*/
  this->setPan(STARTUP_PAN);
  this->setTilt(STARTUP_TILT);

  this->workspace = (char*)calloc(WORKSPACE_SIZE, sizeof(char));
  this->Attached = true;
}

BBBGimble::~BBBGimble()
{
  fclose(this->panServoFP);
  fclose(this->tiltServoFP);
  free(this->workspace);
}

bool BBBGimble::isAttached()
{
  return this->Attached;
}

bool BBBGimble::setPan(double deg)
{
  if (deg < 0 || deg > 180)
  {
    return false;
  }

  double duty = ((deg / 180.) * 1320000.) + 880000; 
  fprintf(stdout, "PanDuty = %d\n", (int)duty);
  fprintf(this->panServoFP, "%d", (int)duty);
  fseek(this->panServoFP, 0, SEEK_SET);
  this->panDuty = duty;
  return true;
}

bool BBBGimble::setTilt(double deg)
{
  if (deg < 0 || deg > 90)
  {
    return false;
  }

  double duty = ((deg / 90.) * 1320000.) + 880000;
  fprintf(stdout, "TiltDuty = %d\n", (int)duty);
  fprintf(this->tiltServoFP, "%d", (int)duty);
  fseek(this->tiltServoFP, 0, SEEK_SET);
  this->tiltDuty = duty;
  return true;
}

bool BBBGimble::stepPan(double deg)
{
  double dutyIncr = (deg / 180.) * 1320000.; 
  double newDuty = dutyIncr + this->panDuty;
  if (newDuty < 880000. || newDuty > 2200000.)
  {
    return false;
  }

  fprintf(this->panServoFP, "%d", (int)newDuty);
  fseek(this->panServoFP, 0, SEEK_SET);
  this->panDuty = newDuty;
  return true;

}

bool BBBGimble::stepTilt(double deg)
{

  double dutyIncr = (deg / 90.) * 1320000; 
  double newDuty = dutyIncr + this->tiltDuty;
  if (newDuty < 880000 || newDuty > 2200000)
  {
    return false;
  }

  fprintf(this->tiltServoFP, "%d", (int)newDuty);
  fseek(this->tiltServoFP, 0, SEEK_SET);
  this->tiltDuty = newDuty;
  return true;
}

double BBBGimble::getPan()
{
  return ( (this->panDuty - 880000.) / 1320000.) * 180;
}

double BBBGimble::getTilt()
{
  return ( (this->tiltDuty - 880000.) / 1320000.) * 90;
}

const char* BBBGimble::processCmd(Command *_C)
{
  if (strcmp(_C->getModule(), "gimble") || (strcmp(_C->getComponent(), "pan") && strcmp(_C->getComponent(), "tilt")))
  {
    return "";
  }

  if (!strcmp(_C->getProperty(), "isAttached"))
  {
    if (this->isAttached())
    {
      return "OK";
    }
    else
    {
      return "ERROR";
    }
  }

  /*For direct set*/
  if (!strcmp(_C->getProperty(), "setDeg"))
  {
    try
    {
      double deg = strtod(_C->getVal(), NULL);
      bool setSuccessfully = false;
      if (!strcmp(_C->getComponent(), "pan"))
      {
        setSuccessfully = this->setPan(deg);
      }
      else
      {
        setSuccessfully = this->setTilt(deg);
      }

      if (setSuccessfully)
      {
        return "OK";
      }
      else
      {
        return "OOR";
      }

    }
    catch (...)
    {
      return "BADARG";
    }
  }

  /*For stepping*/
  if (!strcmp(_C->getProperty(), "stepDeg"))
  {
    try
    {
      double deg = strtod(_C->getVal(), NULL);
      bool setSuccessfully = false;
      if (!strcmp(_C->getComponent(), "pan"))
      {
        setSuccessfully = this->stepPan(deg);
      }
      else
      {
        setSuccessfully = this->stepTilt(deg);
      }

      if (setSuccessfully)
      {
        return "OK";
      }
      else
      {
        return "OOR";
      }

    }
    catch (...)
    {
      return "BADARG";
    }
  }

  /*For getting*/
  if (!strcmp(_C->getProperty(), "getDeg"))
  {
    try
    {
      double retVal = -1;

      if (!strcmp(_C->getComponent(), "pan"))
      {
        retVal = this->getPan();
      }
      else
      {
        retVal = this->getTilt();
      }

      for (int i = 0; i < WORKSPACE_SIZE; i++)
      {
        this->workspace[i] = 0;
      }
      sprintf(this->workspace, "Pos=%lf", retVal);
      const char* retPtr = (const char*)this->workspace;
      return retPtr;
    }
    catch (...)
    {
      return "ERROR";
    }
  }
  return "ERROR";
}

FILE* BBBGimble::initServo(int channel)
{
  FILE* returnFP;

  char folderName[256];
  for (int i = 0; i < 256; i++)
  {
    folderName[i] = 0;
  }

  /*Turn on PWM*/
  if (!this->enabledPWM)
  {
    //system("echo am33xx_pwm > /sys/devices/bone_capemgr.9/slots"); //NO LONGER NEEDED - see /etc/rc.local
    this->enabledPWM = true;
  }

  /*Turn on individual pin*/
  if (channel == 1)
  {
    system("echo bone_pwm_P9_14 > /sys/devices/bone_capemgr.9/slots");
    usleep(1000);
    FILE* fp = popen("ls /sys/devices/ocp.3/ | grep pwm_test_P9_14", "r");
    if (fgets(folderName, sizeof(folderName), fp) == NULL)
    {
      fprintf(stdout, "ERROR: CANNOT FIND FOLDER NAME FOR CHANNEL 1!\n");
      fflush(stdout);
      fclose(fp);
      return NULL;
    }
    char *cptr = folderName;
    for (; *cptr; cptr++)
    {
      if (*cptr == '\n')
      {
        *cptr = 0;
      }
    }
    fprintf(stdout, "Found %s for folder\n", folderName);
    fflush(stdout);
    fclose(fp);
  }
  else if (channel == 2)
  {
    system("echo bone_pwm_P8_13 > /sys/devices/bone_capemgr.9/slots");
    usleep(1000);
    FILE* fp = popen("ls /sys/devices/ocp.3/ | grep pwm_test_P8_13", "r");
    if (fgets(folderName, sizeof(folderName), fp) == NULL)
    {
      fprintf(stdout, "ERROR: CANNOT FIND FOLDER NAME FOR CHANNEL 2!\n");
      fflush(stdout);
      fclose(fp);
      return NULL;
    }
    char *cptr = folderName;
    for (; *cptr; cptr++)
    {
      if (*cptr == '\n')
      {
        *cptr = 0;
      }
    }
    fprintf(stdout, "Found %s for folder\n", folderName);
    fflush(stdout);
    fclose(fp);
    
  }
  else
  {
    fprintf(stdout, "BAD CHANNEL NUMBER: %d\n", channel);
    return NULL;
  }

  /*Turn off while configuring*/
  char cmdBuf[256];
  for (int i = 0; i < 256; i++)
  {
    cmdBuf[i] = 0;
  }

  sprintf(cmdBuf, "echo 0 > /sys/devices/ocp.3/%s/run", folderName);
  system(cmdBuf);
  
  /*Set polarity and period*/
  sprintf(cmdBuf, "echo 0 > /sys/devices/ocp.3/%s/polarity", folderName);
  system(cmdBuf);
  sprintf(cmdBuf, "echo 20000000 > /sys/devices/ocp.3/%s/period", folderName);
  system(cmdBuf);

  /*Set initial duty cycle and initialize file pointer*/
  for (int i = 0; i < 256; i++)
  {
    cmdBuf[i] = 0;
  }
  sprintf(cmdBuf, "/sys/devices/ocp.3/%s/duty", folderName);
  returnFP = fopen(cmdBuf, "w");
  fseek(returnFP, 0, SEEK_SET);
  if (returnFP != NULL)
  {
    fprintf(returnFP, "%d\n", 1000000);
    fflush(returnFP);

    /*Turn on PWM pin*/
    sprintf(cmdBuf, "echo 1 > /sys/devices/ocp.3/%s/run", folderName);
    system(cmdBuf);

    return returnFP;
  }

  fprintf(stdout, "ERROR: CANNOT OPEN DUTY FILE STREAM FOR SERVO %d\n", channel);
  return NULL;

}
