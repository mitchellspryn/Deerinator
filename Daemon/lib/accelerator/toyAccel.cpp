#include "toyAccel.hpp"

toyAccel::toyAccel()
{
  this->attached = false;
  this->fireFP = NULL;

  system("echo 44 > /sys/class/gpio/export");
  usleep(1000);
  system("echo 0 > /sys/class/gpio/gpio44/value");
  usleep(1000);
  system("echo out > /sys/class/gpio/gpio44/direction");
  usleep(1000);

  this->fireFP = fopen("/sys/class/gpio/gpio44/value", "w");
  if (this->fireFP == NULL)
  {
    fprintf(stdout, "Error: Did not successfully open up GPIO44 for toyAccelerator\n");
    return;
  }
  
  this->attached = true;
  return;
}

bool toyAccel::isAttached()
{
  return this->attached;
}

void toyAccel::fire(int ms)
{
  if (this->fireFP == NULL)
  {
    fprintf(stdout, "Error: Attempted to fire with an unattached toyAccelerator\n");
    throw("");
  }

  int fireIter = ms / (2 * FIRE_DUTY_TIME);
  for (int i = 0; i < fireIter; i++)
  {
    fprintf(this->fireFP, "1");
    fseek(this->fireFP, 0, SEEK_SET);
    usleep(FIRE_DUTY_TIME * 1000);
    fprintf(this->fireFP, "0");
    fseek(this->fireFP, 0, SEEK_SET);
    usleep(FIRE_DUTY_TIME * 1000);
  }

  return;
}

const char* toyAccel::processCmd(Command* _C)
{
  if (strcmp(_C->getModule(), "accelerator"))
  {
    fprintf(stdout, "1");
    return "";
  }

  if (strcmp(_C->getComponent(), "fireCtl"))
  {
    fprintf(stdout, "2");
    return "ERROR";
  }

  if (!strcmp(_C->getProperty(), "fire"))
  {
    fprintf(stdout, "Firing for %d ms\n", FIRE_TIME);
    fflush(stdout);
    this->fire(FIRE_TIME);
    return "DONE";
  }

  else if (!strcmp(_C->getProperty(), "isAttached"))
  {
    if (this->attached)
    {
      fprintf(stdout, "4");
      return "READY";
    }
    else
    {
      fprintf(stdout, "5");
      return "ERROR";
    }
  }

  return "ERROR";
}
