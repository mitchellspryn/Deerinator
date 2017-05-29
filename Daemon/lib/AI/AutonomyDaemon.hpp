#ifndef AUTONOMY_DAEMON_HPP
#define AUTONOMY_DAEMON_HPP

#include <stdlib.h>
#include <stdio.h>
#include "accelerator/toyAccel.hpp"
#include "AI/VideoDeerTracker.hpp"
#include "AI/AICameraWrapper.hpp"
#include "gimble/BBB_Gimble.hpp"
#include <opencv2/highgui/highgui.hpp>

#define NUM_POINTS_MIN 25
#define NUM_FRAMES_MIN 2UL

#define HORIZ_FOV 40. //in degrees
#define VERT_FOV 20.  //in degrees

#define DEFAULT_TILT 23.0
#define DEFAULT_PAN  90.0

#define AUTONOMY_DEBUG 1
#define AUTONOMY_SHOW 1

/*Print macro*/
#if AUTONOMY_DEBUG 
  #define AUTONOMY_DEBUG_PRINT(...) \
    fprintf(stdout, __VA_ARGS__); 
#else
  #define AUTONOMY_DEBUG_PRINT(...) \
    do {} while(0);
#endif



#define NUM_STDDEV_X (float)1.5
#define NUM_STDDEV_Y (float)1.5

class AutonomyDaemon
{
  private:
    BBBGimble *gimble;
    VideoDeerTracker *tracker;
    AICameraWrapper *AIC;
    toyAccel *accel;

    std::vector<cv::Point2f> centers;

    float xImgSize;
    float yImgSize;

    bool attached;

    void fireAccelerator(cv::Point target);
    void clearHits();
    void goToDefaultPosition();

  public:
    AutonomyDaemon(BBBGimble *_g, VideoDeerTracker *_vdt, toyAccel *_accel, AICameraWrapper *_AIC);
    ~AutonomyDaemon();

    void runIter();
    bool isAttached();
};

#endif
