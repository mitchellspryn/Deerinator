#ifndef VIDEOSTREAMER_HPP
#define VIDEOSTREAMER_HPP

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <vector>
#include <string.h>
#include "messages.hpp"

#define MESSAGES_DEBUG_MODE_ON

class VideoStreamer
{
  public:
    VideoStreamer();
    ~VideoStreamer();

    bool Initialize();
    bool isInitialized();
    bool isStreaming();
    bool StartStream();
    bool StopStream();

  private:

    void Stream();
    bool isInit;
    bool isStream;
    pid_t PID;
    std::vector<int> killVals;
};

#endif
