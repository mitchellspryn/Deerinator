#include "TCP_Server.hpp"

//using namespace std; //INTERFERES WITH bind() IN sys/types.h

void* TCPServerThreadFxn(void* arg)
{
  fprintf(stdout, "THREAD STARTING...\n");
  TCP_Server *server = (TCP_Server*)arg;
  char buf [512];
  for (int i = 0; i < 512; i++)
  {
    buf[i] = 0;
  }
  /*listen*/
  server->wait_for_connection();

  while(server->isAlive())
  {
    Command *C = new Command(); //TODO: Careful...

    fprintf(stdout, "Entering recieve...\n");
    fflush(stdout);
    server->Receive(buf, 512);  
    
    /*Attempt to parse command*/
    C->Parse(buf);
    if (!C->isValid())
    {
      fprintf(stdout, "ERRORS: ");
      fflush(stdout);
      std::vector<const char*> errors = C->getErr();
      if (errors.size() > 0)
      {
        for (unsigned int i = 0; i < errors.size(); i++)
        {
          fprintf(stdout, "%s ", errors[i]);
          server->Send(errors[i]);
        }
      }
      else
      {
        server->Send("INVALIDCMD\r");
      }
      fprintf(stdout, "\n");
    }
    else
    {
      /*Fire off commands*/
      fprintf(stdout, "Command Parsed: ");
      C->printCmd(stdout);
      std::vector<const char*> results = server->raiseEvent(C);
      for (unsigned int i = 0; i < results.size(); i++)
      {
        if (strlen(results[i]) > 0)
        {
          fprintf(stdout, "SENDING %s\n", results[i]);
          server->Send(results[i]);
        }
      }
    }

    delete C;
  }

  fprintf(stdout, "THREAD EXITING\n");
  fflush(stdout);
  return NULL;
}

TCP_Server::TCP_Server(const char* port)
{
  /* Initialize */
  strncpy(this->port, port, TCPSRV_PORT_SIZE-1);
  this->port[TCPSRV_PORT_SIZE-1] = '\0';
  this->islsock = 0;
  this->iscsock = 0;
  this->killAll = false;
  this->commandListeners = std::vector<std::function<const char*(Command*)> >();
}

TCP_Server::~TCP_Server()
{
  ;
}

std::vector<const char*> TCP_Server::raiseEvent(Command *C)
{
  std::vector<const char*> retVals = std::vector<const char*>();
  for (unsigned int i = 0; i < this->commandListeners.size(); i++)
  {
    retVals.push_back(this->commandListeners[i](C));
  }
  return retVals;
}

int TCP_Server::Start()
{
  /* Create the listen socket */
  if (this->create_listen_socket() == -1)
  {
    ERROR_MSG("failed to create the listen socket");
    return -1;
  }
  this->islsock = 1;


  /* Listen for incoming connections on the specified port */
  if (listen(this->lsocket, 1) == -1)
  {
    ERROR_MSG("failed to listen: %s", strerror(errno));
    this->KillServer();
    return -1;
  }

  /*Create pthread*/
  pthread_mutex_init(&serverMutex, NULL);
  pthread_create(&serverThread, NULL, TCPServerThreadFxn, this);

  return 0;
}

int TCP_Server::KillServer()
{
  if (this->iscsock)
    close(this->csocket);
  this->iscsock = 0;

  if (this->islsock)
    close(this->lsocket);
  this->islsock = 0;

  strcpy(this->port, "");
  pthread_mutex_lock(&serverMutex);
  this->killAll = true;
  pthread_cancel(serverThread);
  pthread_mutex_unlock(&serverMutex);
  pthread_join(serverThread, NULL);
  pthread_mutex_destroy(&serverMutex);
  
  return 0;
}

int TCP_Server::isConnected()
{
  return this->iscsock;
}

bool TCP_Server::isAlive()
{
  return !this->killAll && this->iscsock;
}

int TCP_Server::addListener(std::function<const char*(Command*)> _target)
{
  this->commandListeners.push_back(_target);
  return 0;
}

int TCP_Server::wait_for_connection()
{
  struct sockaddr in_addr;
  socklen_t in_addr_size = sizeof(struct sockaddr);


  /* If a connection already exists, close it first */
  if (this->iscsock)
   this->close_connection();


  /* Wait for a connection, then accept it */
  do
  {
    if ((this->csocket = accept(this->lsocket, &in_addr, &in_addr_size)) == -1)
    {
      WARN_MSG("failed to accept the incoming connection: %s", strerror(errno));
      return -1;
    }
  } while (errno == EAGAIN && !this->killAll);

  if (!this->killAll)
  {
    this->iscsock = 1;
  }

  return 0;
}

void TCP_Server::close_connection()
{
  if (this->iscsock)
    close(this->csocket);
  this->iscsock = 0;
}

int TCP_Server::Receive(char* str, size_t size)
{
  int bytes;


  /* Check that a connection exists */
  if (!(this->iscsock))
  {
    ERROR_MSG("no connection exists");
    strcpy(str, "");
    return -1;
  }

  for (unsigned int i = 0; i < size; i++)
  {
    str[i] = 0;
  }

  /* Receive data */
  fprintf(stdout, "TCP_Server::Receive waiting...\n");
  fflush(stdout);
  bytes = recv(this->csocket, str, size-1, 0); 

  fprintf(stdout, "TCP_Server::Receive activated with string %s\n", str);
  fflush(stdout);
  switch(bytes)
    {
      case -1:
        ERROR_MSG("failed to receive data: %s", strerror(errno));
        strcpy(str, "");
        this->close_connection();
        return -1;
        break;

      case 0:
        /* Connection closed remotely */
        strcpy(str, "");
        this->close_connection();
        return 0;
        break;
    }


  /* If we received data, find and remove the '\n' protocol tail */
  if (str[bytes-1] != '\n')
    {
      ERROR_MSG("packet did not contain \'\\n\' protocol tail");
      strcpy(str, "");
      return -1;
    }
  str[bytes-1] = '\0';


  return 0;
}

int TCP_Server::Send(const char* _str)
{
  char* str = (char*)calloc(strlen(_str) + 1, sizeof(char));

  if (str == NULL)
  {
    return 0;
  }

  strcpy(str, _str);
  int bytes;
  int len = strlen(str);


  /* Check that a connection exists */
  if (!(this->iscsock))
    {
      ERROR_MSG("no connection exists");
      return -1;
    }


  /* Replace the string delimiter '\0' with the '\n' tail and send */
  str[len] = '\n';
  bytes = send(this->csocket, str, len+1, MSG_NOSIGNAL);
  str[len] = '\0';
  switch (bytes)
    {
      case -1:
        ERROR_MSG("failed to send data: %s", strerror(errno));
        this->close_connection();
        free(str);
        return -1;
        break;

      case 0:
        /* Connection closed remotely */
        this->close_connection();
      default:
        free(str);
        return 0;
        break;
    }
}

int TCP_Server::create_listen_socket()
{
  struct addrinfo setup, *srvs;
  int temp;
  int yes = 1;


  /* Setup the listen socket information */
  memset(&setup, 0, sizeof(setup));
  setup.ai_family = AF_UNSPEC;
  setup.ai_socktype = SOCK_STREAM;
  setup.ai_flags = AI_PASSIVE;


  /* Get the address information of sockets for the specified port */
  if ((temp = getaddrinfo(NULL, this->port, &setup, &srvs)) != 0)
    {
      ERROR_MSG("failed to setup the socket: %s", gai_strerror(temp));
      return -1;
    }


  /* Create the listen socket */
  if ((this->lsocket = socket(srvs->ai_family, srvs->ai_socktype,
                           srvs->ai_protocol)) == -1)
    {
      ERROR_MSG("failed to create the listen socket: %s", strerror(errno));
      freeaddrinfo(srvs);
      return -1;
    }
  /* Get's rid of the "Address already in use" error after shutdown */
  setsockopt(this->lsocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));


  /* Bind the listen socket */
  if (bind(this->lsocket, srvs->ai_addr, srvs->ai_addrlen) == -1)
    {
      ERROR_MSG("failed to bind the listen socket: %s", strerror(errno));
      freeaddrinfo(srvs);
      close(this->lsocket);
      return -1;
    }


  freeaddrinfo(srvs);
  return 0;
}
