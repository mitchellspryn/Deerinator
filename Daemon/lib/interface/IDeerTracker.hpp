#ifndef IDEERTRACKER_HPP
#define IDEERTRACKER_HPP

#include <stdlib.h>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.h>
#include <vector>

class IDeerTracker
{
  public:
    virtual void Initialize() = 0;
    virtual bool isAttached() = 0;
    virtual std::vector<cv::Rect> detect(cv::Mat image);
    virtual cv::Mat debugDetectAndDraw(cv::Mat image);
};

#endif
