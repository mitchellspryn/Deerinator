#include "RealCamera.hpp"

using namespace Lunabotics;
using namespace cv;

RealCamera::RealCamera()
{
	this->initialized = false;
  this->CT = UNKNOWN;
}

RealCamera::~RealCamera()
{
  ;
}

bool RealCamera::Initialize(const char* Identifier, const char* CalFile)
{
	fprintf(stdout, "Initializing webcam at %s...\n\n", Identifier);
	clock_t t;
	t = clock();

  if (this->CT == USB_WEBCAM)
  {
    int usbPortNo;
    try
    {
      usbPortNo = (int)strtol(Identifier, NULL, 10);
    }
    catch (...)
    {
      fprintf(stdout, "Error: Bad identifier for USB webcam: %s\n", Identifier);
      return false;
    }

    fprintf(stdout, "usbPortNo = %d\n", usbPortNo);

    this->VC = new VideoCapture(usbPortNo);
    if (!this->VC->isOpened())
    {
      return false;
    }
  }

  else if (this->CT == IP_WEBCAM)
  {
    if (this->VC->open(Identifier))
    {
      return false;
    }
  }

  else
  {
    fprintf(stdout, "Error: Unknown Cameratype for real camera.\n");
    return false;
  }

	/*test*/
	Mat aa;
	this->VC->read(aa);
	fprintf(stdout, "Col = %d, Row = %d\n", aa.cols, aa.rows);

	t = clock() - t;
	fprintf(stdout, "Time to initialize camera: %f\n", ((float)t/CLOCKS_PER_SEC));

  if (CalFile != NULL)
  {
    FileStorage fs (CalFile, FileStorage::READ);
    fs["camera matrix"] >> this->Intrinsics;
    fs["distortion"] >>  this->Distortion;
  }

  t = clock() - t;
  fprintf(stdout, "Time to read parameters: %f\n", ((float)t/CLOCKS_PER_SEC));
	this->initialized = true;
  return true;
}

Mat RealCamera::getImage()
{
	if (this->initialized == false)
		return Mat();//says to return a null one, idk

	Mat SnapShot;	
	//this->VC.set(CV_CAP_PROP_POS_AVI_RATIO, 1);
	this->VC->read(SnapShot);

  /*Check to verify that camera is still attached*/
  if (!SnapShot.data)
  {
    this->initialized = false;
  }
	return SnapShot;
}

bool RealCamera::isAttached()
{
  return this->initialized;
}

Mat RealCamera::getIntrinsics()
{
  return this->Intrinsics;
}

Mat RealCamera::getDistortion()
{
  return this->Distortion;
}

void RealCamera::setCamType(CameraTypes _CT)
{
  this->CT = _CT;
  return;
}
