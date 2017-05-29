#ifndef IMAGE_DEERTRACKER_HPP
#define IMAGE_DEERTRACKER_HPP

#define USE_ANN 0
#define USE_SVM 1

#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "AI/ArrayStack.hpp"
#include "AI/AICameraWrapper.hpp"

#if USE_ANN
#include "fann.h"
#endif
#if USE_SVM
#include "svm.h"
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#define LEARNING_RATE (double)0.1
#define CONTOUR_POINT_SPACING (int)20
#define IMG_SIZE Size(320, 240)

#define NUM_FEATURES 30 

class VideoDeerTracker
{
  public:
    VideoDeerTracker(int _winwidth, int _winheight, float _thresh, char* _annFile, bool _failSilently, bool _debugging, bool _isProb);
    ~VideoDeerTracker();

    bool isValid();

    std::vector<int> getThresholdedResults();
    std::vector<float> getRawResults();
    std::vector<cv::Point> getCenters();

    bool processImageWindow(cv::Mat Image);
    bool processImageKeypoint(cv::Mat Img);


    bool debugViewWindow(int waitMS, cv::Mat img);
    bool debugViewKeyPoint(int waitMS, cv::Mat img);

    void initializeFeed(AICameraWrapper *AIC);

  private:
    int window_width;
    int window_height;
    
    bool valid;
    bool aligned;
    bool failSilently;
    bool debugging;

    cv::Mat extractBackground(cv::Mat input);
    std::vector<cv::Point> getMotionPoints(cv::Mat input);
    std::vector<float> runAIWindow(cv::Mat image, std::vector<cv::Point> centerPoints);
    std::vector<float> runAIKeypoint(cv::Mat image, cv::Mat mask, std::vector<cv::Point> cPoints);

#if USE_ANN
    struct fann *ann;
#endif

#if USE_SVM
    struct svm_model *model;
#endif

    float thresh;

    cv::BackgroundSubtractorMOG bgSubtractor;
    cv::Mat FGMaskMOG; //For use with background subtractor
    
    std::vector<cv::Point> centerPoints;
    std::vector<int> thresholdedResults;
    std::vector<float> rawResults;
    std::vector<cv::KeyPoint> kPoints;
    cv::Mat desc;
    bool isProb;
#if USE_ANN
    std::vector<ArrayStack<float>* > imgChunks; 
#endif

#if USE_SVM
    std::vector<ArrayStack<svm_node>* > imgChunks;
#endif
};

#endif
