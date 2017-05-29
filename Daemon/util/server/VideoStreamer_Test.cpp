#include <stdlib.h>
#include <stdio.h>
#include "server/VideoStreamer.hpp"

int main(void)
{
  VideoStreamer *VS = new VideoStreamer();
  VS->Initialize();
  VS->StartStream();
  char c;
  fscanf(stdin, "%c", &c);
  VS->StopStream();
  return 0;
}
