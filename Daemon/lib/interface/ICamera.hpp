#ifndef ICAMERA_HPP
#define ICAMERA_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class ICamera
{
  public:
    virtual ~ICamera() {;};
    /*call this method to initialize the camera
     * returns true if the camera initialized correctly*/
    /*For USB webcams, Identifier is a numeric string identifing 
     * which USB port to connect to
     *For IP Webcams, Identifier is the IP address of the remote camera*/
    virtual bool Initialize(const char* Identifier, const char* CalFile) = 0;

    /*call this method to retreive an image from the camera
     * returns null if camera is not attached*/
    virtual cv::Mat getImage() = 0;

    /*call this method to determine if the camera is initialized*/
    virtual bool isAttached() = 0;

    virtual cv::Mat getIntrinsics() = 0;

    virtual cv::Mat getDistortion() = 0;
};
#endif
