#ifndef IMAGE_DEERTRACKER_HPP
#define IMAGE_DEERTRACKER_HPP

#define USE_ANN 0
#define USE_SVM 1

#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "AI/ArrayStack.hpp"

#if USE_ANN
  #include "fann.h"
#endif
#if USE_SVM
  #include "svm.h"
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class ImageDeerTracker
{
  public:
    ImageDeerTracker(int _iwidth, int _iheight, int _winwidth, int _winheight, int _step, float _thresh, char* _annFile, bool _failSilently);
    ~ImageDeerTracker();

    bool isAligned();
    bool isValid();
    
    bool* getThresholdedResults();
    float* getRawResults();
    int getNumWindows();

    bool processImage(cv::Mat Image);

  private:
    int window_width;
    int window_height;
    int step_size;
    int numHoriz;
    int numVert;
    int numWin;
    int image_width;
    int image_height;
    int hExclude;
    int vExclude;
    int hBoxesPerDiv;
    int vBoxesPerDiv;
    
    bool valid;
    bool aligned;
    bool failSilently;

#if USE_ANN
    struct fann *ann;
#endif

#if USE_SVM
    struct svm_model *model;
#endif

    float *imgResults;
    bool *threshImgResults;

    float thresh;
    
#if USE_ANN
    std::vector<ArrayStack<float>* > imgChunks; 
#endif

#if USE_SVM
    std::vector<ArrayStack<svm_node>* > imgChunks;
#endif
};

#endif
