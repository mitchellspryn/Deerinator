#ifndef AI_CAMERA_WRAPPER_HPP
#define AI_CAMERA_WRAPPER_HPP

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <AI/OCVCapture.h>
#include <pthread.h>

#define WAIT_US 100000

class AICameraWrapper
{
  private:
    pthread_mutex_t thread_mutex;
    pthread_t thread;

    bool keepRunning;
    cv::Mat latestImage;
    OCVCapture *VC;
    bool activated;

  public:
    AICameraWrapper(int index);
    ~AICameraWrapper();
    
    bool isStillRunning();
    bool read(cv::Mat &M);
    bool isActivated();
    bool kill();
    void setImage(cv::Mat M);
    cv::Mat getLatest(); 
};

#endif
