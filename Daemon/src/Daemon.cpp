/**
 * @file Daemon.cpp
 * @author	Mitchell Spryn
 *
 * @brief	The main entry point for the application
 */



#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <ncurses.h>
#include <ctype.h>
#include <vector>
#include "server/VideoStreamer.hpp"
#include "server/Command.hpp"
#include "server/TCP_Server.hpp"
#include "gimble/BBB_Gimble.hpp"
#include "accelerator/toyAccel.hpp"
#include "AI/VideoDeerTracker.hpp"
#include "AI/AutonomyDaemon.hpp"
#include "AI/AICameraWrapper.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp> //for videocapture

using namespace std;
using namespace cv;

/** Size of the buffers for sending and receiving data */
static const size_t SERVER_BUFFER_SIZE = 5000;

/** Server port */
static const char* SERVER_PORT = "54861";

#define AI_FILE "/root/SVMs/oneClassSIFT_n.8.machine"
#define AI_THRESH .8

/** TCP Server Object */
TCP_Server *t;

toyAccel *accel;
BBBGimble *gimble;
VideoDeerTracker *tracker;
AICameraWrapper *AIC;
AutonomyDaemon *ad;


/*Hardware initialization function*/
int initHardware();


static int handle_connection()
{
  /*Killing Autonomy Daemon*/
  fprintf(stdout, "Killing Autonomy Daemon...\n");
  delete ad;
  delete AIC;
  delete tracker;
  /*Start Video Stream*/
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
    return 1;
  }

  /*Block here while the user is still connected*/
  while (t->isConnected()) {usleep(10000);};
  
  /*Kill the stream*/
  VS->StopStream();
  delete VS;

  /*Starting daemon...*/
  fprintf(stdout, "Restarting daemon...\n");

  /*Create camera wrapper*/
  AIC = new AICameraWrapper(0); 
  if (!AIC->isActivated())
  {
    fprintf(stdout, "Error: could not open videocapture on port 0\n");
    delete AIC;
    delete gimble;
    delete accel;
    return 1;
  }

  /*Create tracker*/
  tracker = new VideoDeerTracker(64, 48, AI_THRESH, AI_FILE, false, true, true);
  if (!tracker->isValid())
  {
    fprintf(stdout, "Invalid videoTracker. Stopping...\n");
    delete AIC; 
    delete gimble;
    delete accel;
    delete tracker;
    return 1;
  }

  tracker->initializeFeed(AIC);

  /*Create autonomy daemon*/
  ad = new AutonomyDaemon(gimble, tracker, accel, AIC);
  if (!ad->isAttached())
  {
    fprintf(stdout, "Error: could not create autonomy daemon.\n");
    delete ad;
    delete AIC;
    delete gimble;
    delete accel;
    delete tracker;
    return 1;
  }

  fprintf(stdout, "Restarted!\n");
  return 0;
}

int autonomous_iteration()
{
  /*TODO: Continue operating autonomously*/
  fprintf(stdout, "Starting another Autonomous iter\n");
  ad->runIter();
  return 0;
}


/*The main entry point of the application*/
int main(void)
{
  if (!initHardware())
  {
    return 0;
  }

  /*Start the server*/
  t = new TCP_Server(SERVER_PORT);
  if (t->Start() == -1)
  {
    fprintf(stderr, "failed to start the server\n");
    return EXIT_FAILURE;
  }

  fprintf(stdout, "Attaching listeners...");
  function<const char*(Command*)> gimbleCallback = bind(&BBBGimble::processCmd, gimble, std::placeholders::_1);
  function<const char*(Command*)> accelCallback = bind(&toyAccel::processCmd, accel, std::placeholders::_1);
  t->addListener(gimbleCallback);
  t->addListener(accelCallback);
  fprintf(stdout, "Done!\n");

  /* Keep handling incoming connections, one at a time */
  while (1)
    {
      /* Wait for an incoming connection */
      printf("Waiting for connection on TCP port %s...\n", t->port);

      if (t->isConnected() == -1)
        handle_connection();
      else
        autonomous_iteration();
    }


  t->KillServer();
  delete t;
  return 0;
}

int initHardware()
{

  /*Create gimble*/
  fprintf(stdout, "Creating gimble with pan servo attached to channel 1 and tilt servo attached to channel 2...");
  gimble = new BBBGimble(2);
  if (!gimble->isAttached())
  {
    fprintf(stdout, "Can't attach gimble. Killing everything and dying...\n");
    delete gimble;
    return 0;
  }
  fprintf(stdout, "Done!\n");

  /*Create accelerator*/
  fprintf(stdout, "Creating accelerator...");
  accel = new toyAccel();
  if (!(accel->isAttached()))
  {
    fprintf(stdout, "Error: coult not attached accelerator. Killing everything and dying\n");
    delete gimble;
    delete accel;
    return 0;
  }
  fprintf(stdout, "Done!\n");

  /*Create camera wrapper*/
  fprintf(stdout, "Opening camera wrapper...");
  AIC = new AICameraWrapper(0); 
  if (!AIC->isActivated())
  {
    fprintf(stdout, "Error: could not open videocapture on port 0\n");
    delete AIC;
    delete gimble;
    delete accel;
    return 0;
  }
  fprintf(stdout, "Done!\n");

  /*Create tracker*/
  fprintf(stdout, "Creating videoTracker...");
  tracker = new VideoDeerTracker(64, 48, AI_THRESH, AI_FILE, false, true, true);
  if (!tracker->isValid())
  {
    fprintf(stdout, "Invalid videoTracker. Stopping...\n");
    delete AIC; 
    delete gimble;
    delete accel;
    delete tracker;
    return 0;
  }

  fprintf(stdout, "Initializing the feed...\n");
  tracker->initializeFeed(AIC);

  /*Create autonomy daemon*/
  fprintf(stdout, "Creating autonomy daemon...");
  ad = new AutonomyDaemon(gimble, tracker, accel, AIC);
  if (!ad->isAttached())
  {
    fprintf(stdout, "Error: could not create autonomy daemon.\n");
    delete ad;
    delete AIC;
    delete gimble;
    delete accel;
    delete tracker;
    return 0;
  }

  fprintf(stdout, "Done!\n");
  return 1;
}
