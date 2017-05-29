#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "AI/ImageDeerTracker.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
  if (argc < 4)
  {
    fprintf(stdout, "Usage: $./singleImage.cpp <image> <entity> <thresh>\n");
    fprintf(stdout, "\t<image> : image on which to run the AI entity\n");
    fprintf(stdout, "\t<entity> : AI file (either libSVM or FANN model\n");
    fprintf(stdout, "\t<thresh> : Detection threshold. Between -1 and 1\n");
    return 0;
  }

  int stepSize = 1;

  fprintf(stdout, "Reading image from %s...", argv[1]);
  Mat image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  fprintf(stdout, "Done!\nReading threshhold from %s...", argv[3]);
  double thresh = strtod(argv[3], NULL);
  fprintf(stdout, "Thresh is %f\nCreating new DeerTracker from %s...", thresh, argv[2]);
  ImageDeerTracker *D = new ImageDeerTracker(image.cols, image.rows, 64, 48, stepSize, thresh, argv[2], false);

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

  fprintf(stdout, "Done!\n Running...\n");
  clock_t t = clock();
  D->processImage(image);
  t = clock() - t;
  fprintf(stdout, "Done! Processing took %f seconds\nDrawing results...", (float)t / (float)CLOCKS_PER_SEC);
  
  /*Draw the boxes*/
  float* rawResults = D->getRawResults();
  bool* threshResults = D->getThresholdedResults();
  Mat allDataOutput;
  cvtColor(image, allDataOutput, CV_GRAY2RGB);
  Mat threshOutput;
  cvtColor(image, threshOutput, CV_GRAY2RGB);
  for (int i = 0; i < D->getNumWindows(); i++)
  {
    Point startingPoint, endingPoint, midPoint;
    startingPoint.x = (i % (image.cols-64)) * stepSize;
    startingPoint.y = (i / (image.cols-64)) * stepSize;
    endingPoint.x = startingPoint.x + 64;
    endingPoint.y = startingPoint.y + 48;
    midPoint.x = (startingPoint.x + endingPoint.x) / 2;
    midPoint.y = (startingPoint.y + endingPoint.y) / 2;

    if (threshResults[i])
    {
      Point startingPoint, endingPoint;
      //rectangle(threshOutput, startingPoint, endingPoint, Scalar(255, 0, 0), 2);
      circle(threshOutput, midPoint, stepSize, Scalar(255, 0, 0), 2);
      fprintf(stdout, "Hit at (%d, %d) with value of %f\n", midPoint.x, midPoint.y, rawResults[i]);
    }

    //circle(allDataOutput, midPoint, stepSize, Scalar( 127. + (127. * (1-rawResults[i])), -1, 127. + (127. * (1-rawResults[i]))));

  }

  /*Show results*/
  fprintf(stdout, "Done!\nShowing...");
  namedWindow("threshOutput", CV_WINDOW_AUTOSIZE);
  //namedWindow("rawOutput", CV_WINDOW_AUTOSIZE);
  imshow("threshOutput", threshOutput);
  //imshow("rawOutput", allDataOutput);
  waitKey(0);

  /*Clean up*/
  fprintf(stdout, "Done!\nCleaning up...\n");
  //delete D; TODO: Figure out why this causes double free?
  fprintf(stdout, "Done!\nNormal termination.\n");
  return 0;
}
