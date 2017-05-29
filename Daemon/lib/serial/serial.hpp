#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
//#include <sys/stats.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#include <pthread.h>
#include <functional>
#include <vector>

#define SERIAL_PORT "/dev/tty01"
#define BAUDRATE B9600

class serial
{
  public:
    serial();
    ~serial();
    bool isEnabled();
    bool isAttached();
    int sendData(char* data);
    void attachCallback(std::function<void(char*)> target);
    void event(char* data);
    int getfd();

  private:
    int fd;
    struct termios tio;
    struct termios old_tio;
    std::vector<std::function<void(char*)> > callbacks;
    pthread_t serialThread;
    bool enabled;
    bool attached;

};

#endif
