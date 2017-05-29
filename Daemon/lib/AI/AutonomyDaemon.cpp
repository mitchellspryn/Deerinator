#include "AutonomyDaemon.hpp"

using namespace std;
using namespace cv;

AutonomyDaemon::AutonomyDaemon(BBBGimble *_g, VideoDeerTracker *_vdt, toyAccel *_accel, AICameraWrapper *_AIC)
{
  this->attached = false;
  this->gimble = _g;
  this->tracker = _vdt;
  this->AIC = _AIC;
  this->accel = _accel;

  if (!this->AIC->isActivated())
  {
    fprintf(stdout, "ERROR: AutonomyDaemon given unopened AICameraWrapper\n");
  }

  Mat frame;
  if (!this->AIC->read(frame))
  {
    fprintf(stdout, "ERROR: Cannot read from given AICameraWrapper\n");
    return;
  }

  /*These are destined to be used in floating point computations*/
  this->xImgSize = (float)frame.cols;
  this->yImgSize = (float)frame.rows;

  this->centers = vector<Point2f>();

  this->goToDefaultPosition();

  this->attached = true;
  return;
}

AutonomyDaemon::~AutonomyDaemon()
{
  /*Nothing to do here, handled in main thread!*/
}

bool AutonomyDaemon::isAttached()
{
  return this->attached;
}

void AutonomyDaemon::runIter()
{
  /*Get the next video frame*/
  AUTONOMY_DEBUG_PRINT("Starting new iter...\n");
  Mat nextFrame;
  if (!this->AIC->read(nextFrame))
  {
    fprintf(stdout, "ERROR: AutonomyDaemon could not read image from AICameraWrapper\n");
    return;
  }

  //fprintf(stdout, "Showing image...\n");
  //namedWindow("IMG", CV_WINDOW_AUTOSIZE);
  //imshow("IMG", nextFrame);
  //waitKey(0);

  /*Process the image*/
  this->tracker->processImageKeypoint(nextFrame);

  /*Grab the centers*/
  vector<Point> imageHits = this->tracker->getCenters();

#if AUTONOMY_SHOW
  this->tracker->debugViewWindow(1000, nextFrame);
#endif

  /*If we have a large enough number of hits, compute center of mass and save*/
  if (imageHits.size() > NUM_POINTS_MIN)
  {
    AUTONOMY_DEBUG_PRINT("I see something...\n");
    float cX = 0;
    float cY = 0;
    for (unsigned int i = 0; i < imageHits.size(); i++)
    {
      cX += imageHits[i].x;
      cY += imageHits[i].y;
    }

    cX /= (float)imageHits.size();
    cY /= (float)imageHits.size();

    this->centers.push_back(Point(cX, cY));
    
#if AUTONOMY_DEBUG
    AUTONOMY_DEBUG_PRINT("Centers: <");
    for (unsigned int jj = 0; jj < this->centers.size()-1; jj++)
    {
      AUTONOMY_DEBUG_PRINT("(%f, %f)|", this->centers[jj].x, this->centers[jj].y);
    }
    AUTONOMY_DEBUG_PRINT("(%f, %f)>\n", this->centers[this->centers.size() - 1].x, this->centers[this->centers.size() - 1].y);
#endif

    float stddevX = 0;
    float stddevY = 0;
    for (unsigned int i = 0; i < imageHits.size(); i++)
    {
      stddevX += (imageHits[i].x-cX)*(imageHits[i].x-cX);
      stddevY += (imageHits[i].y-cY)*(imageHits[i].y-cY);
    }

    stddevX = sqrt(stddevX/(float)imageHits.size());
    stddevY = sqrt(stddevY/(float)imageHits.size());
    AUTONOMY_DEBUG_PRINT("Current stdev: (x,y) = (%f, %f)\n", stddevX, stddevY);

    /*Verify that the new center is within NUM_STDDEV standard deviations of the previous center*/
    if (this->centers.size() > 1 && abs(this->centers[this->centers.size() - 2].x - cX) > NUM_STDDEV_X * stddevX)
    {
      AUTONOMY_DEBUG_PRINT("X outside of stddev requirement. We have teleporting deer. Stopping\n"); 
      this->clearHits();
    }

    else if (this->centers.size() > 1 && abs(this->centers[this->centers.size() - 2].y- cY) > NUM_STDDEV_Y * stddevY)
    {
      AUTONOMY_DEBUG_PRINT("X outside of stddev requirement. We have teleporting deer. Stopping\n"); 
      this->clearHits();
    }

    else
    {
      AUTONOMY_DEBUG_PRINT("This iter OK. We have %ld hits\n", this->centers.size());
      /*See if we have enough frames to justify firing. If so, start fire sequence*/
      if (this->centers.size() >= NUM_FRAMES_MIN)
      {
        AUTONOMY_DEBUG_PRINT("We have enough. Staring firing sequence\n");      
        this->fireAccelerator(this->centers[this->centers.size() - 1]);
        AUTONOMY_DEBUG_PRINT("Going back to the default position\n");
        this->goToDefaultPosition();
        AUTONOMY_DEBUG_PRINT("Clearing after successful firing\n");
        this->clearHits();
        
      }
    }
  }
  
  else
  {
    AUTONOMY_DEBUG_PRINT("I did not see anything in this frame. Clearing...\n");
    this->clearHits();
  }
}

void AutonomyDaemon::fireAccelerator(Point target)
{
  /*Compute the amount that the servos need to move*/
  AUTONOMY_DEBUG_PRINT("-----STARTING FIRE SEQUENCE-----\n");
  float dX = target.x - this->xImgSize/2.;
  float dY = target.y - this->yImgSize/2.;

  float dYaw = dX * (HORIZ_FOV/this->xImgSize);
  float dPitch = dY * (VERT_FOV/this->yImgSize);

  AUTONOMY_DEBUG_PRINT("\tdPitch, dYaw = (%f, %f)\n", dPitch, dYaw);

  /*Move the servos*/
  this->gimble->stepPan(dYaw);
  usleep(100000);
  this->gimble->stepTilt(dPitch);
  usleep(100000);

  /*FIRE TEH LAZORZ!!!1!!!one!*/
  AUTONOMY_DEBUG_PRINT("\tFIRING!\n");
  this->accel->fire(1000);
  AUTONOMY_DEBUG_PRINT("\tDone firing\n");
  return;
}


void AutonomyDaemon::clearHits()
{
  AUTONOMY_DEBUG_PRINT("Clearing hits\n");
  this->centers.clear();
  return;
}

void AutonomyDaemon::goToDefaultPosition()
{
  AUTONOMY_DEBUG_PRINT("Going to default position of (pan, tilt) = (%lf, %lf)\n", DEFAULT_PAN, DEFAULT_TILT);
  this->gimble->setPan(DEFAULT_PAN);
  this->gimble->setTilt(DEFAULT_TILT);
  return;
}
