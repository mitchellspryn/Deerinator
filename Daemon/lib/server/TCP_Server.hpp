#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#include "messages.hpp"
#include "interface/IServer.hpp"
#include "server/Command.hpp"
#include <functional>
#include <vector>

/** Max string size of a TCP port number (5 digits + '\0') */
#define TCPSRV_PORT_SIZE 6


class TCP_Server : public IServer
{

  public:
    char port[TCPSRV_PORT_SIZE]; /**< Port number */

    int lsocket;	/**< Listen socket descriptor */
    int islsock;  /**< Is the listen socket active */

    int csocket;	/**< Connection socket descriptor */
    int iscsock;  /**< Is the connection socket active */


    TCP_Server(const char* port);
    ~TCP_Server();
    int Start();
    int Send(const char* str);
    int Receive(char* str, size_t size);
    int KillServer();
    int isConnected();
    bool isAlive();
    int addListener(std::function<const char*(Command*)> _target);
    std::vector<const char*> raiseEvent(Command *C);

    int wait_for_connection();
    
    
  private:
  /**
   * Create the server's listen socket.
   *
   * @param	s	Server
   *
   * @return -1 on failure
   */
  int create_listen_socket();
  void close_connection();
  bool killAll;
  std::vector<std::function<const char*(Command*)> > commandListeners;
  pthread_t serverThread;
  pthread_mutex_t serverMutex;
};
#endif
