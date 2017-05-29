#include <stdlib.h>
#include <stdio.h>
#include <accelerator/toyAccel.hpp>

int main(void)
{
  fprintf(stdout, "Attaching toy accelerator...");

  toyAccel *accel = new toyAccel();
  if (!accel->isAttached())
  {
    fprintf(stdout, "Couldn't attach. Stopping...\n");
    return 0;
  }

  fprintf(stdout, "Done! Firing for 500 ms...");
  accel->fire(500);
  
  fprintf(stdout, "Done! Catching my breath...\n");
  usleep(2000000);

  fprintf(stdout, "Firing for 1 second...");
  accel->fire(1000);
  fprintf(stdout, "Done! Cleaning up and terminating.\n");

  delete accel;
  return 0;
}
