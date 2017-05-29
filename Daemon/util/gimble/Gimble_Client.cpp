#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "gimble/BBB_Gimble.hpp"
#include "interface/IGimble.hpp"

void printState(BBBGimble *g)
{
  fprintf(stdout, "Device is at (%lf, %lf)\n", g->getPan(), g->getTilt());
  fflush(stdout);
  return;
}

int main(void)
{
  fprintf(stdout, "Initializing gimble...");
  fflush(stdout);

  BBBGimble *gimble = new BBBGimble(2); //Pan -> 1, tilt -> 2

  fprintf(stdout, "Done!\n");
  fprintf(stdout, "Use WASD for control...\n");
  fflush(stdout);

  bool done = false;
  double stepSize = 5.0;

  double gimblePan = 0;
  double gimbleTilt = 0;

  while(!done)
  {
    char c = getchar();
    fprintf(stdout, "Read %c\n", c);
    fprintf(stdout, "\n");
    switch (c)
    {
      case 'w':
        gimblePan += stepSize;
        gimble->setPan(gimblePan);
        printState(gimble);
        break;
      case 's':
        gimblePan -= stepSize;
        gimble->setPan(gimblePan);
        printState(gimble);
        break;
      case 'a':
        gimbleTilt += stepSize;
        gimble->setTilt(gimbleTilt);
        printState(gimble);
        break;
      case 'd':
        gimbleTilt -= stepSize;
        gimble->setTilt(gimbleTilt);
        printState(gimble);
        break;
      case 'q':
        done = true;
        break;
      default:
        fprintf(stdout, "Unrecognized character: %c\n", c);
        break;
    }
  }

  fprintf(stdout, "Cleaning up...\n");
  delete gimble;
  fprintf(stdout, "Normal termination.\n");
  fflush(stdout);
  return 0;
}
