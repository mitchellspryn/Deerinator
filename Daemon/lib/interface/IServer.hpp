#ifndef ISERVER_HPP
#define ISERVER_HPP

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class IServer
{
  public:
    virtual ~IServer() {;};
    virtual int Start() = 0;
    virtual int KillServer() = 0;
    virtual int isConnected() = 0;
    virtual int wait_for_connection() = 0;
    virtual void close_connection() = 0;
    virtual int Send(const char* str) = 0;
    virtual int Receive(char *data, size_t size) = 0;
};

#endif
