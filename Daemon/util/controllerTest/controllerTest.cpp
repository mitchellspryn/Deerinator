#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <ncurses.h>
#include <ctype.h>
#include "server/VideoStreamer.hpp"
#include "server/Command.hpp"
#include "server/TCP_Server.hpp"
#include "gimble/BBB_Gimble.hpp"
#include "accelerator/toyAccel.hpp"

using namespace std;

int main(void)
{
  /*Start Video Stream*/
  fprintf(stdout, "Skipping video stream for now...\n");
  fprintf(stdout, "Starting video stream...");
  VideoStreamer *VS = new VideoStreamer();
  VS->Initialize();
  VS->StartStream();
  if (VS->isStreaming())
  {
    fprintf(stdout, "Stream started!\n");
  }
  else
  {
    fprintf(stdout, "Stream not started. Killing everything and dying...\n");
    VS->StopStream();
    delete VS;
    return 0;
  }

  /*Create gimble*/
  fprintf(stdout, "Creating gimble with pan servo attached to channel 1 and tilt servo attached to channel 2...");
  BBBGimble *gimble = new BBBGimble(2);
  if (!gimble->isAttached())
  {
    fprintf(stdout, "Can't attach gimble. Killing everything and dying...\n");
    delete gimble;
    VS->StopStream();
    delete VS;
    return 0;
  }
  fprintf(stdout, "Done!\n");

  /*Create accelerator*/
  fprintf(stdout, "Creating accelerator...");
  toyAccel *accel = new toyAccel();
  if (!(accel->isAttached()))
  {
    fprintf(stdout, "Error: coult not attached accelerator. Killing everything and dying\n");
    delete gimble;
    delete accel;
    VS->StopStream();
    delete VS;
    return 0;
  }
  fprintf(stdout, "Done!\n");

  
  /*Create TCP/IP Server*/
  fprintf(stdout, "Creating TCP/IP Server on port 54861...");
  TCP_Server *server = new TCP_Server("54861");
  
  fprintf(stdout, "Server created.\n");

  fprintf(stdout, "Attaching listeners...");
  function<const char*(Command*)> gimbleCallback = bind(&BBBGimble::processCmd, gimble, std::placeholders::_1);
  function<const char*(Command*)> accelCallback = bind(&toyAccel::processCmd, accel, std::placeholders::_1);
  server->addListener(gimbleCallback);
  server->addListener(accelCallback);
  fprintf(stdout, "Done!\n");

  fprintf(stdout, "Starting server in background...");
  server->Start();
  fprintf(stdout, "Server started.\n");

  fprintf(stdout, "Waiting for async commands. Press 'q' to shut down\n");
  fflush(stdout);
  int keyInput = 0;
  while(keyInput != 'q')
  {
    keyInput = getchar();
    if (isprint(keyInput))
    {
      fprintf(stdout, "Read %c\n", keyInput);
      fflush(stdout);
    }
  }

  fprintf(stdout, "Shutting down server...");
  server->KillServer();
  delete server;
  fprintf(stdout, "Server down.\n");
  
  fprintf(stdout, "Shutting down video feed...");
  VS->StopStream();
  delete VS;
  fprintf(stdout, "Video feed should be (theoretically) stopped.\n");

  fprintf(stdout, "Shutting down gimble...");
  delete gimble;
  fprintf(stdout, "Gimble killed.\n");

  fprintf(stdout, "Safely terminating.\n");
  
  return 0;
}
