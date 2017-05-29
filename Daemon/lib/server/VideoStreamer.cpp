#include "VideoStreamer.hpp"

using namespace std;

VideoStreamer::VideoStreamer()
{
  this->isInit = false;
  this->isStream = false;
  this->killVals = vector<int>();
  return;
}

VideoStreamer::~VideoStreamer()
{
  return;
}

bool VideoStreamer::Initialize()
{
  this->isInit = true;
  return true;
}

bool VideoStreamer::isInitialized()
{
  return this->isInit;
}

bool VideoStreamer::isStreaming()
{
  return this->isStream;
}

bool VideoStreamer::StartStream()
{
  DEBUG_MSG("Starting stream...");
  this->Stream();
  return true;
}

bool VideoStreamer::StopStream()
{
  DEBUG_MSG("Stopping stream...");
  char ProcessBuf[2048];
  for (int i = 0; i < 2048; i++)
  {
    ProcessBuf[i] = 0;
  }
  DEBUG_MSG("Killing process %d", this->PID);
  sprintf(ProcessBuf, "kill -9 %d", this->PID);
  DEBUG_MSG("Running %s", ProcessBuf);
  system(ProcessBuf);
  for (unsigned int i = 0; i < this->killVals.size(); i++)
  {
    sprintf(ProcessBuf, "kill -9 %d", this->killVals[i]);
    DEBUG_MSG("Running %s", ProcessBuf);
    system(ProcessBuf);
  }
  this->isStream = false;
  DEBUG_MSG("Should be dead...");
  return true;
}

void VideoStreamer::Stream()
{
  this->PID = fork();
  if (this->PID != 0)
  {
    //Find PIDs to kill
    sleep(3);
    DEBUG_MSG("Checking for PIDs...");
    this->killVals.clear();
    FILE *fp;
    fp = popen("ps aux", "r");
    char processBuf[2048];
    for (int i = 0; i < 2048; i++)
    {
      processBuf[i] = 0;
    }
    if (fp != NULL)
    {
      while (fgets(processBuf, sizeof(processBuf), fp) != NULL)
      {
        DEBUG_MSG("Next Line: %s", processBuf);
        /*Parse output*/
        vector<char*> tokens;
        char* tok = strtok(processBuf, " ");
        while(tok != NULL)
        {
          DEBUG_MSG("\tNextToken = %s", tok);
          if (strlen(tok) != 0)
          {
            DEBUG_MSG("\t\tAdding %s", tok);
            tokens.push_back(tok);
          }

          tok = strtok(NULL, " ");
        }

        for (unsigned int i = 0; i < tokens.size(); i++)
        {
          DEBUG_MSG("-->%s", tokens[i]);
        }

        if (tokens.size() > 11)
        {
          char* dummyptr = strstr(tokens[10], "mjpg_streamer");
          if (dummyptr != NULL)
          {
            int pid = (int)strtol(tokens[2], NULL, 10);
            DEBUG_MSG("Found pid of %d", pid);
            this->killVals.push_back(pid);
          }
        }
      }
    }
    DEBUG_MSG("No more!");
    
    DEBUG_MSG("PIDs:");
    for (unsigned int i = 0; i < this->killVals.size(); i++)
    {
      DEBUG_MSG("%d", this->killVals[i]);
    }
    this->isStream = true;
    return;
  }
  DEBUG_MSG("Child starting stream...");
  //system("/home/mitchell/ece492/Deerinator_Software/Deerinator_Software/trunk/Daemon/lib/mjpg-streamer/mjpg-streamer/start.sh");
  system("/root/programs/Daemon/lib/mjpg-streamer/mjpg-streamer/start.sh");
  while (1)
  {
    DEBUG_MSG(".");
    sleep(1);
  }
  ERROR_MSG("ERROR: Stream() process ended when it shouldn't.");
  return;
}
