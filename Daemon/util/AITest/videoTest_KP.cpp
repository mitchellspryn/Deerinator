#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "AI/VideoDeerTracker.hpp"
#include "AI/AICameraWrapper.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
  if (argc < 4)
  {
    fprintf(stdout, "Usage: $./videoTest.cpp <videoStream> <entity> <thresh>\n");
    fprintf(stdout, "\t<videoStream> : video on which to run the AI entity\n");
    fprintf(stdout, "\t<entity>      : AI file (either libSVM or FANN model\n");
    fprintf(stdout, "\t<thresh>      : Detection threshold. Between -1 and 1\n");
    fprintf(stdout, "\t<-p>          : enable probabilistic thresholding\n");
    fprintf(stdout, "\t<-l>          : use a live video feed\n");
    return 0;
  }

  bool isProb = false;
  bool isLive = false;
  for (int i = 0; i < argc; i++)
  {
    if (!strcmp(argv[i], "-p"))
    {
      isProb = true;
    }

    if (!strcmp(argv[i], "-l"))
    {
      isLive = true;
    }
  }

  VideoCapture v;
  AICameraWrapper *AIC;
  if (!isLive)
  {
    fprintf(stdout, "Opening Videocapture from %s...", argv[1]);

    v = VideoCapture(argv[1]);
    if (!v.isOpened())
    {
      fprintf(stdout, "Can't open. Stopping...\n");
      return 0;
    }
  }
  else
  {
    fprintf(stdout, "Attempting to convert %s to integer...", argv[1]);
    int index = (int)strtol(argv[1], NULL, 10);
    fprintf(stdout, "got %d...", index);
    AIC = new AICameraWrapper(index);
    if (!AIC->isActivated())
    {
      fprintf(stdout, "Can't open. Stopping...\n");
      return 0;
    }
    
  }

  fprintf(stdout, "Done!\nReading threshhold from %s...", argv[3]);
  double thresh = strtod(argv[3], NULL);
  fprintf(stdout, "Thresh is %f\nCreating new DeerTracker from %s...", thresh, argv[2]);
  VideoDeerTracker *D = new VideoDeerTracker(64, 48, thresh, argv[2], false, true, isProb);

  if (!D->isValid())
  {
    fprintf(stdout, "Invalid DeerTracker. Stopping...\n");
    delete D;
    return 0;
  }


  fprintf(stdout, "Done!\n Running...\n");
  Mat frame, frame_gray;

  if (!isLive)
  {
    while(v.read(frame))
    {
      cvtColor(frame, frame_gray, CV_RGB2GRAY); 
      D->processImageKeypoint(frame_gray);
      D->debugViewKeyPoint(1, frame_gray);
    }
  }
  else
  {
    while(AIC->read(frame_gray))
    {
      D->processImageKeypoint(frame_gray);
      D->debugViewKeyPoint(1, frame_gray);
    }
  }
  
  return 0;
}
