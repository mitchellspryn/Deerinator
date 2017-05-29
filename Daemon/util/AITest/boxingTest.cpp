#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "AI/ImageDeerTracker.hpp"

using namespace std;
using namespace cv;

int main(void)
{
  fprintf(stdout, "Generating mat...");
  Mat M = Mat(480, 640, CV_8UC1);
  for (int i = 0; i < 480; i++)
  {
    for (int j = 0; j < 640; j++)
    {
      M.at<uchar>(i, j) = ((i*16) + j)%255;
    }
  }

  //cout << endl << M << endl; 

  fprintf(stdout, "Done generating %dx%d mat!\n", M.rows, M.cols);
  fprintf(stdout, "Creating DeerTracker with 4x4 window and 2 pixel step size...");

  //DeerTracker *D = new DeerTracker(16, 16, 4, 4, 2, 0, "ANN.dat", false);
  ImageDeerTracker *D = new ImageDeerTracker(640, 480, 64, 48, 8, 0, "ANN.dat", false);
  
  if (!D->isValid())
  {
    fprintf(stdout, "Invalid DeerTracker. Stopping...\n");
    delete D;
    return 0;
  }

  if (!D->isAligned())
  {
    fprintf(stdout, "Alignment error. Stopping...\n");
    delete D;
    return 0;
  }
  
  fprintf(stdout, "Done!\n");
  fprintf(stdout, "Processing image (I hope you remembered to use GDB...)");

  D->processImage(M);

  fprintf(stdout, "Done!\n");
  fprintf(stdout, "Cleaning up safely (Check this with valgrind)...\n");

  delete D;

  fprintf(stdout, "Normal termination.\n");
  return 0;
}
