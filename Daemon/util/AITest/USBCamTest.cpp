#include <stdlib.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int main(void)
{
  fprintf(stdout, "Opening videocapture on port 0...\n");
  VideoCapture v = VideoCapture(0);

  if (!v.isOpened())
  {
    fprintf(stdout, "Can't open. Stopping...\n");
    return 0;
  }

  namedWindow("out", CV_WINDOW_AUTOSIZE);
  Mat frame;
  while(1)
  {
    v.read(frame);
    imshow("out", frame);
    waitKey(1);
  }

  return 0;
}
