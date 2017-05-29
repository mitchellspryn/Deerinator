#include "serial.hpp"

using namespace std;

void* serialThreadFxn(void* arg)
{
  serial* s = (serial*)arg;
  int s_fd = s->getfd();

  char data[256];
  for (int i = 0; i < 256; i++)
  {
    data[i] = 0;
  }

  while(s->isEnabled())
  {
    size_t numRead = read(s_fd, data, 256);
    if (numRead)
    {
      s->event(data);
    }
  }
  
  pthread_exit(NULL);
}

serial::serial()
{
  this->callbacks = vector<function<void(char*)> >();

  /*Load the pin config*/
  system("echo uart1 > /sys/devices/bone_capemgr.9/slots");

  /*Open file stream*/
  this->fd = open(SERIAL_PORT, O_RDWR|O_NOCTTY);
  if (this->fd < 0)
  {
    fprintf(stdout, "Warning: cannot open serial port\n");
    return;
  }

  tcgetattr(this->fd, &this->old_tio);

  bzero(&this->tio, sizeof(&this->tio));

  /*Set up 8n1 at specified baudrate*/
  this->tio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  this->tio.c_iflag = IGNPAR;
  this->tio.c_oflag = 0;
  this->tio.c_lflag = ICANON;

  /*Flush the current settings and apply the new ones*/
  tcflush(this->fd, TCIFLUSH);
  tcsetattr(this->fd, TCSANOW, &this->tio);

  /*Start listener thread*/
  this->enabled = true;
  pthread_create(&serialThread, NULL, serialThreadFxn, this);

  this->attached = true;
}

serial::~serial()
{
  this->enabled = false;
  pthread_cancel(serialThread);
  pthread_join(serialThread, NULL);
  tcsetattr(this->fd, TCSANOW, &this->old_tio);
}

int serial::getfd()
{
  return this->fd;
}

bool serial::isEnabled()
{
  return this->enabled;
}

bool serial::isAttached()
{
  return this->attached;
}

int serial::sendData(char* data)
{
  int totalSent = 0;
  int numToSend = strlen(data);
  while(totalSent < numToSend)
  {
    int result = write(this->fd, data, numToSend);
    if (result == -1)
    {
      fprintf(stdout, "Error sending data.\n");
      return -1;
    }
    totalSent += result;
  }

  return totalSent;
}

void serial::attachCallback(std::function<void(char*)> target)
{
  this->callbacks.push_back(target);
  return;
}

void serial::event(char* data)
{
  for (unsigned int i = 0; i < this->callbacks.size(); i++)
  {
    this->callbacks[i](data);
  }
}
