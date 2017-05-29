#include "VideoDeerTracker.hpp"

using namespace std;
using namespace cv;

VideoDeerTracker::VideoDeerTracker(int _winwidth, int _winheight, float _thresh, char* _aiFile, bool _failSilently, bool _debugging, bool _isProb)
{
  this->valid = false;
  this->aligned = false;
  this->failSilently = _failSilently;
  this->debugging = _debugging;
  this->isProb = _isProb;

#if USE_ANN
  /*Attempt to load ANN, failing if it can't be read*/
  this->ann = fann_create_from_file(_aiFile);
  if (this->ann == NULL)
  {
    if (!_failSilently)
    {
      throw("UNABLE TO READ ANN FROM FILE");
    }
    return;
  }
#endif

#if USE_SVM
  /*Attempt to load SVM, failing if it can't be read*/
  fprintf(stdout, "Trying to load...");
  fflush(stdout);
  this->model = svm_load_model(_aiFile);
  fprintf(stdout, "Loaded!\n");
  fflush(stdout);
  if (model == NULL)
  {
    if (!_failSilently)
    {
      throw("UNABLE TO READ SVM FROM FILE");
    }
    return;
  }
#endif

  /*Set all internal class variables*/
  this->window_width = _winwidth;
  this->window_height = _winheight;
  this->thresh = _thresh;

  this->bgSubtractor =  BackgroundSubtractorMOG(); //TODO: Determine if this needs to be subtractorMOG or subtractorMOG2

  if (this->debugging)
  {
    namedWindow("Original", CV_WINDOW_AUTOSIZE);
    namedWindow("Mask", CV_WINDOW_AUTOSIZE);
    namedWindow("keyPoints", CV_WINDOW_AUTOSIZE);
    namedWindow("Targets", CV_WINDOW_AUTOSIZE);
  }
  this->valid = true;

  /*All done!*/
  return;
}

VideoDeerTracker::~VideoDeerTracker()
{
  return;
}

bool VideoDeerTracker::isValid()
{
  return this->valid;
}

vector<int> VideoDeerTracker::getThresholdedResults()
{
  return this->thresholdedResults;
}

vector<float> VideoDeerTracker::getRawResults()
{
  return this->rawResults;
}

vector<Point> VideoDeerTracker::getCenters()
{
  return this->centerPoints;
}

bool VideoDeerTracker::debugViewWindow(int waitMS, Mat img)
{
  if (img.channels() != 3)
  {
    cvtColor(img, img, CV_GRAY2RGB);
  }
  
  Mat img2, imgTarget, imgOrig;
  resize(img, imgOrig, IMG_SIZE, 0, 0, INTER_AREA);
  resize(img, img2, IMG_SIZE, 0, 0, INTER_AREA); 
  resize(img, imgTarget, IMG_SIZE, 0, 0, INTER_AREA);

  /*Draw centers*/
  int hits = 0;
  for (unsigned int i = 0; i < this->centerPoints.size(); i++)
  {
    int red = (int)(127.0 + (this->rawResults[i] * 127.0));
    int blue = 255 - red;
    circle(img2, this->centerPoints[i], 3, Scalar(red, 0, blue), -1);

    /*Draw AI hits as well*/
    if (this->thresholdedResults[i])
    {
      circle(img2, this->centerPoints[i], 10, Scalar(0, 255, 0), 3);
      circle(imgTarget, this->centerPoints[i], 10, Scalar(0, 255, 0), 3);
      rectangle(imgTarget, Point(this->centerPoints[i].x - (this->window_width/2), this->centerPoints[i].y - (this->window_height/2)), Point(this->centerPoints[i].x + (this->window_width/2), this->centerPoints[i].y + (this->window_height/2)), Scalar(255, 0, 0), 2, 8);
      hits++;
    }
  }

  /*Show images*/
  imshow("Original", imgOrig);
  imshow("keyPoints", img2);
  imshow("Targets", imgTarget);
  imshow("Mask", this->FGMaskMOG);
  //fprintf(stdout, "Size mask = (%d, %d) ; Size Original = (%d, %d)\n", this->FGMaskMOG.rows, this->FGMaskMOG.cols, imgOrig.rows, imgOrig.cols);
  fprintf(stdout, "%d hits\n", hits);
  //fflush(stdout);

  waitKey(waitMS);

  return true; 
}

bool VideoDeerTracker::debugViewKeyPoint(int waitMS, Mat img)
{
  if (img.channels() != 3)
  {
    cvtColor(img, img, CV_GRAY2RGB);
  }
  
  Mat img2, imgTargets, imgOriginal;
  resize(img, imgOriginal, IMG_SIZE, 0, 0, INTER_AREA);
  resize(img, img2, IMG_SIZE, 0, 0, INTER_AREA); 
  resize(img, imgTargets, IMG_SIZE, 0, 0, INTER_AREA);

  /*Draw keyPoints*/
  vector<KeyPoint> goodK = vector<KeyPoint>();
  vector<KeyPoint> badK = vector<KeyPoint>();

  for (unsigned int i = 0; i < this->kPoints.size(); i++)
  {
    if ((int)(this->thresholdedResults[i])>0.5)
    {
      goodK.push_back(this->kPoints[i]);
    }
    else
    {
      badK.push_back(this->kPoints[i]);
    }
  }

  //fprintf(stdout, "%ld ; %ld\n", goodK.size(), this->thresholdedResults.size());
  drawKeypoints(imgTargets, goodK, imgTargets, Scalar(0, 255, 0), 4); //4->draw rich keypoints 
  drawKeypoints(img2, goodK, img2, Scalar(0, 255, 0), 4);
  drawKeypoints(img2, badK, img2, Scalar(0, 0, 255), 4);
  //for (unsigned int i = 0; i < this->kPoints.size(); i++)
  //{
  //  circle(img2, this->kPoints[i].pt, this->kPoints[i].size, Scalar(255, 0, 0), -1);
  //}

  /*Show images*/
  imshow("Original", imgOriginal);
  imshow("keyPoints", img2);
  imshow("Targets", imgTargets);
  imshow("Mask", this->FGMaskMOG);

  waitKey(waitMS);

  return true; 
}

bool VideoDeerTracker::processImageWindow(cv::Mat Img)
{
  Mat Img2(Img);
  //resize(Img, Img2, IMG_SIZE, 0, 0, INTER_AREA);
  Mat Extracted = this->extractBackground(Img2);
  vector<Point> cPoints = this->getMotionPoints(Extracted);
  vector<float> fPts = this->runAIWindow(Img, cPoints);
  return true;
}

bool VideoDeerTracker::processImageKeypoint(cv::Mat Img)
{
  //fprintf(stdout, "Got image...\n");
  Mat Mask = this->extractBackground(Img);
  //fprintf(stdout, "Extracted background...\n");
  vector<Point> cPoints = this->getMotionPoints(Mask);
  //fprintf(stdout, "Got centers...\n");
  vector<float> fpoints = this->runAIKeypoint(Img, Mask, cPoints);
  //fprintf(stdout, "got points!\n");
  return true;
}

Mat VideoDeerTracker::extractBackground(cv::Mat input)
{
  this->bgSubtractor(input, this->FGMaskMOG, LEARNING_RATE);
  return this->FGMaskMOG;
}

vector<Point> VideoDeerTracker::getMotionPoints(Mat eroded)
{
  /*Find contours in the input image*/
  //Mat eroded; 
  //erode(input, eroded, Mat(), Point(-1, -1), 1);
  //Mat eroded(input);
  //bilateralFilter(input, eroded, 5, 20, 20);
  vector<vector<Point> > contours;
  vector<Vec4i> h;
  //fprintf(stdout, "Size of img = (%d, %d)\n", eroded.rows, eroded.cols);
  //fflush(stdout);
  findContours(eroded, contours, h, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0));
  //fprintf(stdout, "Found %ld contours\n", contours.size());
  //fflush(stdout);

  this->centerPoints = vector<Point>();  
  
  for (unsigned int i = 0; i < contours.size(); i++)
  {
    if (contours[i].size() > CONTOUR_POINT_SPACING)
    {
      int numPointsInContour = (contours[i].size() / CONTOUR_POINT_SPACING) - 1;
      for (int j = 0; j < numPointsInContour; j++)
      {
        Point targetPoint = contours[i][j*CONTOUR_POINT_SPACING];

        /*Bounds check*/
        if (targetPoint.x < (this->window_width/2))
        {
          //targetPoint.x = this->window_width / 2;
          continue;
        }
        if (targetPoint.y < (this->window_height/2))
        {
          //targetPoint.y = this->window_height / 2;
          continue;
        }

        if (targetPoint.x > eroded.cols - (this->window_width / 2))
        {
          //targetPoint.x =  eroded.cols - (this->window_width / 2);
          continue;
        }
        if (targetPoint.y > eroded.rows - (this->window_height / 2))
        {
          //targetPoint.y =  eroded.rows - (this->window_height / 2);
          continue;
        }

        this->centerPoints.push_back(targetPoint);
      }
    }
  }

  //fprintf(stdout, "%ld center points\n", this->centerPoints.size());
  //fflush(stdout);

  return this->centerPoints;
}

vector<float> VideoDeerTracker::runAIWindow(Mat image, vector<Point> centers)
{
  this->thresholdedResults = vector<int>();
  this->rawResults = vector<float>();

#if USE_ANN
  float data[this->window_width*this->window_height];
#endif
#if USE_SVM
  svm_node data[(this->window_width*this->window_height) + 1];
#endif

  float mProb = -1;
  for (unsigned int i = 0; i < centers.size(); i++)
  {
    /*Marshall pixels*/
    int cx = centers[i].x;
    int cy = centers[i].y;
    int count = 0;
    for (int j = -(window_height/2); j < (window_height/2); j++)
    {
      for (int k = -(window_width/2); k < (window_width/2); k++)
      {
#if USE_ANN
        data[count] = (float)image.at<uchar>(cy+j, cx+k);
#endif
#if USE_SVM
        svm_node n;
        n.index = count;
        n.value = ((double)image.at<uchar>(cy+j, cx+k) - 127.) / 127.; 
        data[count] = n;
        count++;
#endif
      }
    }

    //Mat AA(window_height, window_width, CV_8UC1);
    //for (int j = 0; j < window_height; j++)
    //{
    //  for (int k = 0; k < window_width; k++)
    //  {
    //    AA.at<uchar>(j, k) = data[k + (j*window_width)];
    //  }
    //}

    //imshow("aa", AA);
    //waitKey(0);

    /*Run AI*/
#if USE_ANN
    float *fptr = fann_run(this->ann, data);
    this->rawResults.push_back(*fptr);
    this->thresholdedResults.push_back(*fptr >= this->thresh);
#endif

#if USE_SVM
    svm_node n;
    n.index = -1;
    n.value = 0;
    data[this->window_width*this->window_height] = n;
    //double res = svm_predict(this->model, data);

    double prob;
    double res;
    if (this->isProb)
    {
      res = svm_predict_probability(this->model, data, &prob);
      this->rawResults.push_back((float)prob);
      this->thresholdedResults.push_back(prob >= this->thresh && res > 0);
      if (mProb < prob)
      {
        mProb = prob;
      }
    }
    else
    {
      res = svm_predict(this->model, data);
      fprintf(stdout, "res = %f\n", res);
      this->rawResults.push_back(res);
      this->thresholdedResults.push_back(res>0.5);
    }

#endif

  }
  
  return this->rawResults;
}

/*TODO: implement*/
vector<float> VideoDeerTracker::runAIKeypoint(Mat image, Mat mask, vector<Point> cPoints)
{
  /*Extract SIFT keypoints*/
  this->kPoints.clear();
  this->desc = Mat();
  this->rawResults.clear();
  this->thresholdedResults.clear();
  for (unsigned int i = 0; i < cPoints.size(); i++)
  {
    /*TODO: Figure out correct sizing*/
    for (int j = 3; j < 5; j++)
    {
      KeyPoint k;
      k.pt = cPoints[i];
      k.size = pow(2, j);
      this->kPoints.push_back(k);
    }
  }
  SIFT S(NUM_FEATURES, 4, .04, 10, 1.6);
  S(image, mask, this->kPoints, this->desc, true); //TODO: Maybe use provided keypoints?

  //fprintf(stdout, "Num keypoints: %ld\n", this->kPoints.size());
  //fprintf(stdout, "Size of desc: (%d, %d)\n", this->desc.rows, this->desc.cols);
  //fflush(stdout);

  /*Marshall into SVM*/
  float mProb = -1;
  for (int i = 0; i < this->desc.rows; i++)
  {
    svm_node data[129];
    for (int j = 0; j < this->desc.cols; j++)
    {
      data[j].index = j;
      data[j].value = (this->desc.at<float>(i, j) - 127.)/127.;
    }
    data[128].index = -1;
    data[128].value = -1;
    double prob;
    double res;
    if (this->isProb)
    {
      res = svm_predict_probability(this->model, data, &prob);
      fprintf(stdout, "res = %lf ; prob = %lf\n", res, prob);
      this->rawResults.push_back((float)prob);
      this->thresholdedResults.push_back(prob >= this->thresh && res > 0);
      if (mProb < prob)
      {
        mProb = prob;
      }
    }
    else
    {
      res = svm_predict(this->model, data);
      fprintf(stdout, "res = %lf\n", res);
      this->rawResults.push_back(res);
      if (res>0)
      {
        this->thresholdedResults.push_back(1);
      }
      else
      {
        this->thresholdedResults.push_back(0);
      }
    }
    
  }

  return vector<float>();
}

void VideoDeerTracker::initializeFeed(AICameraWrapper *AIC)
{
  for (int i = 0; i < 20; i++)
  {
    Mat m;
    AIC->read(m);
    this->bgSubtractor(m, this->FGMaskMOG, LEARNING_RATE);
  }
}
