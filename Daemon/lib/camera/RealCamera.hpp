#ifndef REALCAMERA_HPP
#define REALCAMERA_HPP

#include <unistd.h>
#include <Vision/Vision_Library.hpp>
#include "Interface/ICamera.hpp"

#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv/cxcore.h>

//Basic Implementation of the ICamera interface for USB type cameras
//can be extended for use with IP cameras by addition of another initialize function

namespace Lunabotics
{
  typedef enum CameraTypes
  {
    USB_WEBCAM,
    IP_WEBCAM,
    UNKNOWN
  } CameraTypes;

	class RealCamera: public ICamera
	{
	public:
		RealCamera();
    ~RealCamera();

		/*call this method to initialize the camera
		* returns true if the camera initialized correctly*/
		virtual bool Initialize(const char* Identifier, const char* CalFile);

		/*call this method to retreive an image from the camera
		* returns null if camera is not attached*/
		virtual cv::Mat getImage();

		/*call this method to determine if the camera is initialized*/
		virtual bool isAttached();

    cv::Mat getIntrinsics();
    cv::Mat getDistortion();

    void setCamType(CameraTypes _CT);

	private:
		bool initialized;
		cv::VideoCapture *VC;
    CameraTypes CT;
    cv::Mat Intrinsics;
    cv::Mat Distortion;
	};
}
#endif
