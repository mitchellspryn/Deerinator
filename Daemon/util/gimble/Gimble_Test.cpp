#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "gimble/BBB_Gimble.hpp"
#include "interface/IGimble.hpp"

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    fprintf(stdout, "Usage: ./Gimble_Test <steps> <wait>\n");
    fprintf(stdout, "\t<steps> : number of steps for each servo to take between the ranges\n");
    fprintf(stdout, "\t<wait> : us to wait between command changes\n");
    return 0;
  }

  char* endptr;
  char* endptr2;
  double steps = strtod(argv[1], &endptr);
  double wait = strtod(argv[2], &endptr2);
  if (endptr == argv[1])
  {
    fprintf(stdout, "ERROR: cannot convert %s to double. Stopping...\n", argv[1]);
    return 0;
  }

  if (endptr2 == argv[2])
  {
    fprintf(stdout, "ERROR: cannot convert %s to double. Stopping...\n", argv[2]);
    return 0;
  }

  fprintf(stdout, "Using %lf steps...\n", steps);
  fprintf(stdout, "Initializing gimble...");
  fflush(stdout);

  BBBGimble *gimble = new BBBGimble(2); //Pan -> 1, tilt -> 2

  fprintf(stdout, "Done!\n");
  fprintf(stdout, "Starting pan tests...\n");
  fflush(stdout);

  double deg = 0;
  while(deg <= 180.)
  {
    fprintf(stdout, "Setting pan servo to %f degrees directly...", deg);
    fflush(stdout);
    gimble->setPan(deg);
    fprintf(stdout, "Device reports %lf degrees\n", gimble->getPan()); 
    fflush(stdout);
    usleep(wait);
    deg += (180. / steps);
  }

  deg = 0;
  fprintf(stdout, "Setting pan servo to 0 degrees directly...\n");
  fflush(stdout);
  gimble->setPan(deg);
  fprintf(stdout, "Device reports %lf degrees\n", gimble->getPan()); 
  fflush(stdout);
  usleep(wait);

  double stepVal = 180. / steps;
  while (deg <= 180.)
  {
    fprintf(stdout, "Stepping pan servo %f degrees...", stepVal);
    fflush(stdout);
    gimble->stepPan(stepVal);
    fprintf(stdout, "Device reports %lf degrees\n", gimble->getPan()); 
    fflush(stdout);
    usleep(wait);
    deg += stepVal;
  }

  fprintf(stdout, "Pan tests concluded. Starting tilt tests...\n");
  fflush(stdout);

  deg = 0;
  while(deg <= 90.)
  {
    fprintf(stdout, "Setting tilt servo to %f degrees directly...\n", deg);
    fflush(stdout);
    gimble->setTilt(deg);
    fprintf(stdout, "Device reports %lf degrees\n", gimble->getTilt()); 
    fflush(stdout);
    usleep(wait);
    deg += (90. / steps);
  }

  deg = 0;
  fprintf(stdout, "Setting tilt servo to 0 degrees directly...");
  fflush(stdout);
  gimble->setTilt(deg);
  fprintf(stdout, "Device reports %lf degrees\n", gimble->getTilt()); 
  fflush(stdout);
  usleep(wait);

  stepVal = 90. / steps;
  while (deg <= 90.)
  {
    fprintf(stdout, "Stepping tilt servo %f degrees...", stepVal);
    fflush(stdout);
    gimble->stepTilt(stepVal);
    fprintf(stdout, "Device reports %lf degrees\n", gimble->getTilt()); 
    fflush(stdout);
    usleep(wait);
    deg += stepVal;
  }
  
  fprintf(stdout, "Done with tilt test. Terminating...\n");
  delete gimble;

  return 0;
}
