#include "AICameraWrapper.hpp"

using namespace cv;

void* AICameraWrapperThreadFxn(void* arg)
{
  AICameraWrapper* AIC = (AICameraWrapper*)arg;
  
  Mat m;
  while(AIC->isStillRunning())
  {
    if (AIC->read(m))
    {
      AIC->setImage(m);
    }
    usleep(WAIT_US);
  }

  pthread_exit(NULL);
}

AICameraWrapper::AICameraWrapper(int index)
{
  this->activated = false;
  this->keepRunning = false;
  this->VC = new OCVCapture;
  this->VC->setDesiredSize(320, 240);
  this->VC->open();

  if (!this->VC->isOpen())
  {
    fprintf(stdout, "Error: Cannot open OCVCapture on index %d\n", index);
    delete this->VC;
    return; 
  }

  /*Drop black pixels*/
  for (int i = 0; i < 20; i++)
  {
    this->VC->grab();
    usleep(1000);
  }

  this->keepRunning = true;
  pthread_mutex_init(&this->thread_mutex, NULL);
  pthread_create(&this->thread, NULL, AICameraWrapperThreadFxn, this);
  
  fprintf(stdout, "Thread starting!\n");
  this->activated = true;
}

AICameraWrapper::~AICameraWrapper()
{
  this->kill();
  pthread_join(this->thread, NULL);
  pthread_mutex_destroy(&this->thread_mutex);
  delete this->VC;
}

bool AICameraWrapper::isStillRunning()
{
  return this->keepRunning;
}

bool AICameraWrapper::read(cv::Mat &M)
{
  bool returnValue = false;
  pthread_mutex_lock(&this->thread_mutex);
  this->VC->grab();
  returnValue = this->VC->gray(M);
  pthread_mutex_unlock(&this->thread_mutex);
  return returnValue;
}

bool AICameraWrapper::isActivated()
{
  return this->activated;
}

bool AICameraWrapper::kill()
{
  this->keepRunning = false;
  return true;
}

void AICameraWrapper::setImage(cv::Mat M)
{
  pthread_mutex_lock(&this->thread_mutex);
  this->latestImage = M;
  pthread_mutex_unlock(&this->thread_mutex);
  return;
}

Mat AICameraWrapper::getLatest()
{
  return this->latestImage;
}
