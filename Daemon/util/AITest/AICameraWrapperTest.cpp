#include <stdlib.h>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "AI/AICameraWrapper.hpp"

using namespace std;
using namespace cv;

int main(void)
{
  fprintf(stdout, "Creating Camera Wrapper...");
  AICameraWrapper *AIC = new AICameraWrapper(0);
  
  if (!AIC->isActivated())
  {
    fprintf(stdout, "Can't activate...\n");
    return 0;
  }
  
  fprintf(stdout, "Done!\nPress c to grab frame, q to quit\n");
  namedWindow("win", CV_WINDOW_AUTOSIZE);
  while(1)
  {
    char c = getchar();
    if (c == 'q')
    {
      break;
    }

    else if (c == 'c')
    {
      fprintf(stdout, "Grabbing latest...\n");
      Mat M = AIC->getLatest();
      fprintf(stdout, "\tSz = (%d, %d)\n", M.rows, M.cols);
      imshow("win", M);
      waitKey(100);
    }
  }

  fprintf(stdout, "Attempting to gracefully terminate\n");
  delete AIC;
  fprintf(stdout, "Normal termination\n");
  return 0;
}
