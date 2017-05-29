#include "ImageDeerTracker.hpp"

using namespace std;
using namespace cv;

ImageDeerTracker::ImageDeerTracker(int _iwidth, int _iheight, int _winwidth, int _winheight, int _step, float _thresh, char* _aiFile, bool _failSilently)
{
  this->valid = false;
  this->aligned = false;
  this->failSilently = _failSilently;

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
  this->model = svm_load_model(_aiFile);
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
  this->image_width = _iwidth;
  this->image_height = _iheight;
  this->window_width = _winwidth;
  this->window_height = _winheight;
  this->step_size = _step;
  this->thresh = _thresh;

  /*Compute internal class terms*/
  this->numHoriz = ((this->image_width - this->window_width) / this->step_size) + 1;
  this->numVert = ((this->image_height - this->window_height) / this->step_size) + 1;
  this->numWin = this->numHoriz * this->numVert;
  this->hBoxesPerDiv = this->window_width / this->step_size;
  this->vBoxesPerDiv = this->window_height / this->step_size;

  /*Check for consistancy*/
  this->aligned = true;
  if ( ( ((this->image_width - this->window_width) % this->step_size) != 0) || (((this->image_height - this->window_height) % this->step_size) != 0) )
  {
    this->aligned = false;
  }

  if ( ((this->hBoxesPerDiv % this->step_size) != 0) || ((this->vBoxesPerDiv % this->step_size) != 0) )
  {
    //this->aligned = false; //TODO: WTF was I thinking here?
  }

  /*Allocate memory*/
  int i = 0;
  int numPixels = this->window_width * this->window_height;

  try
  {
#if USE_ANN
    for (i = 0; i < this->numWin; i++) 
    {
      ArrayStack<float> *A = new ArrayStack<float>(numPixels);
      if (A == NULL || !A->isValid())
      {
        throw("Can't alloc!");
      }
      A->reset(); //for safety
      this->imgChunks.push_back(A);
    }
#endif

#if USE_SVM
    for (i = 0; i < this->numWin; i++)
    {
      ArrayStack<svm_node> *A = new ArrayStack<svm_node>(numPixels + 1);
      if (A == NULL || !A->isValid())
      {
        throw("Can't alloc!");
      }
      A->reset();
      this->imgChunks.push_back(A);
    }
#endif

    this->imgResults = (float*)calloc(this->numWin, sizeof(float));
    this->threshImgResults = (bool*)calloc(this->numWin, sizeof(float));
    this->valid = true;
  }

  catch (...) //bad_alloc, primarily
  {
    /*free existing memory*/
    int j = i;
    for (int i = 0; i < j; i++)
    {
      free(this->imgChunks[i]);
    }

    if (this->imgResults != NULL)
    {
      free(this->imgResults);
    }
    if (this->threshImgResults != NULL)
    {
      free(this->threshImgResults);
    }

    /*Re-throw exception if needed*/
    this->valid = false;
    if (!this->failSilently)
    {
      throw("CANNOT ALLOCATE MEMORY");
    }

    this->valid = false;
  }

  /*All done!*/
  return;
}

ImageDeerTracker::~ImageDeerTracker()
{
  /*Note: if not valid, memory freed in constructor*/
  if (this->valid)
  {
    /*Free all the memory!*/
    for (int i = 0; i < this->numWin; i++)
    {
      delete this->imgChunks[i];
    }
    free(this->imgResults);
    free(this->threshImgResults);
  }

  return;
}

bool ImageDeerTracker::isAligned()
{
  return this->aligned; 
}

bool ImageDeerTracker::isValid()
{
  return this->valid;
}

bool* ImageDeerTracker::getThresholdedResults()
{
  return this->threshImgResults;
}

float* ImageDeerTracker::getRawResults()
{
  return this->imgResults;
}

int ImageDeerTracker::getNumWindows()
{
  return this->numWin;
}

bool ImageDeerTracker::processImage(cv::Mat Img)
{
  if (Img.rows != this->image_height && Img.cols != this->image_width)
  {
    throw("RUNTIME ERROR: WRONG DIMENSIONS");
  }

  if (Img.type() != CV_8UC1)
  {
    throw("RUTIME ERROR: WRONG INPUT IMAGE TYPE (EXPECTED CV_8UC1)");
  }

  /*Reset all arraystacks*/
  for (unsigned int i = 0; i < this->imgChunks.size(); i++)
  {
    this->imgChunks[i]->reset();
  }

  /*Marshall pixels into rows*/
  for (int i = 0; i < this->image_height; i++)
  {
    for (int j = 0; j < this->image_width; j++)
    {
      /*Find the starting box number*/
      int pixelCol = j;
      int pixelRow = i;
      int horizRemainder = pixelCol % this->step_size;
      int horizAligned = pixelCol - horizRemainder;
      int vertRemainder = pixelRow % this->step_size;
      int vertAligned = pixelRow - vertRemainder;

      int startingBoxNo = (horizAligned + (vertAligned * this->numHoriz)) / this->step_size;
      int hExclude = 0;
      int vExclude = 0;
      int pcol = horizAligned;
      int prow = vertAligned;

      while(pcol >= (this->numHoriz) * this->step_size)
      {
        startingBoxNo--;
        hExclude++;
        pcol -= this->step_size;
      }

      while( prow >= (this->numVert) * this->step_size)
      {
        startingBoxNo -= this->numHoriz;
        vExclude++;
        prow -= this->step_size;
      }

      int lowHExclude = this->hBoxesPerDiv - (pixelCol / this->step_size) - 1;
      if (lowHExclude < 0)
      {
        lowHExclude = 0;
      }

      int lowVExclude = this->vBoxesPerDiv - (pixelRow / this->step_size) - 1;
      if (lowVExclude < 0)
      {
        lowVExclude = 0;
      }

      /*Compute the number of horizontal and vertical boxes in which the pixel resides*/
      int numHoriz = this->hBoxesPerDiv - hExclude - lowHExclude;
      int numVert = this->vBoxesPerDiv - vExclude - lowVExclude;

      /*Marshal pixel*/
#if USE_ANN
      float data = (float)Img.at<uchar>(pixelRow, pixelCol);
      while(numVert > 0)
      {
        int numHorizCopy = numHoriz;
        int index = startingBoxNo - ( (numVert-1) * this->numHoriz);
        while(numHorizCopy > 0)
        {
          this->imgChunks[index]->push(data);
          numHorizCopy--;
          index--;
        }
        numVert--;
      }
#endif
#if USE_SVM
      svm_node data;
      /*Can't fill in data.index here*/
      data.value = ((double)Img.at<uchar>(pixelRow, pixelCol) - 127.) / 127.; //Data should be scaled to be between -1 and 1
      //fprintf(stdout, "Pushing %f (from %lf)\n", data.value, (double)Img.at<uchar>(pixelRow, pixelCol));
      while(numVert > 0)
      {
        int numHorizCopy = numHoriz;
        int index = startingBoxNo - ( (numVert-1) * this->numHoriz);
        while(numHorizCopy > 0)
        {
          data.index = this->imgChunks[index]->nextIndex() + 1;
          this->imgChunks[index]->push(data);
          numHorizCopy--;
          index--;
        }
        numVert--;
      }
#endif
      //fprintf(stdout, "%d%d ", numHoriz, numVert);

      
    }
    //fprintf(stdout, "\n\n");
  }

  

#if USE_SVM
  svm_node footer;
  footer.index = -1;
  footer.value = 0.0;
  for (unsigned int i = 0; i < this->imgChunks.size(); i++)
  {
    this->imgChunks[i]->push(footer);
  }
#endif

#if USE_ANN
  /*Run ANN*/
  float* result = this->imgResults;
  bool* threshResults = this->threshImgResults;
  namedWindow("Box", CV_WINDOW_AUTOSIZE);
  for (int i = 0; i < this->numWin; i++)
  {
    result = fann_run(this->ann, this->imgChunks[i]->data());
    /*Some testing stuff*/
    Mat m (48, 64, CV_8UC1);
    float* data = this->imgChunks[i]->data();
    for (int j = 0; j < 48; j++)
    {
      for (int k = 0; k < 64; k++)
      {
        m.at<uchar>(j, k) = (uchar)(data[k + (64 * j)]);
      }
    }
    imshow("Box", m);
    waitKey(1);
    *threshResults = (*result >= this->thresh);
    result++;
    threshResults++;
  }
#endif

#if USE_SVM
  /*Run SVM*/
  //namedWindow("Box", CV_WINDOW_AUTOSIZE);
  for (int i = 0; i < this->numWin; i++)
  {
    double result = svm_predict(this->model, this->imgChunks[i]->data());
    /*some testing stuff*/
    //Mat m (48, 64, CV_8UC1);
    //struct svm_node* data = this->imgChunks[i]->data();
    //for (int j = 0; j < 48; j++)
    //{
    //  for (int k = 0; k < 64; k++)
    //  {
        //m.at<uchar>(j, k) = (uchar)((data[k + (64 * j)].value * 127.) + 127.);
        //fprintf(stdout, "Got back %d from %lf\n", m.at<uchar>(j, k), data[k + (64*j)].value);
        //fflush(stdout);
    //  }
    //}
    //imshow("Box", m);
    //waitKey(1);
    //fprintf(stdout, ".");

    this->imgResults[i] = result;
    this->threshImgResults[i] = (result >= this->thresh);
  }
#endif

  /*All done :)*/
  return true;
}
