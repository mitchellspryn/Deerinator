/**
 * @file	tcpsrv_test.c
 * @author	Mitchell Spryn
 *
 * @brief	Simple funcionality tests for tcpsrv.h/c.
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server/TCP_Server.hpp"
#include "interface/IServer.hpp"


/** Size of the buffers for sending and receiving data */
static const size_t SERVER_BUFFER_SIZE = 5000;

/** Server port */
static const char* SERVER_PORT = "54861";

/** TCP Server Object */
TCP_Server *t;



/**
 * Handles a connection to the server.
 *
 * @details	This function is called when the command server
 * 		receives a new connection.  This function loops,
 * 		accepting strings of commands, passing them to the
 * 		robot for parsing, receiving any response, and
 * 		forwarding the response back to the client.  The
 * 		function returns only when the client closes the
 * 		connection.
 */
static int handle_connection()
{
  char recv[SERVER_BUFFER_SIZE];
  char resp[SERVER_BUFFER_SIZE];


  /* Handle the connection until the remote system closes it */
  while (t->IsConnected())
    {
      /* Receive data from the client */
      if (t->Receive(recv, SERVER_BUFFER_SIZE) == -1)
        fprintf(stderr, "failed to receive data\n");


      /* Pass the command string to the control system */
      strcpy(resp, "echo: ");
      strncat(resp, recv, SERVER_BUFFER_SIZE - 6);
      //resp[SERVER_BUFFER_SIZE-1];


      /* Send the response back to the client */
      if (t->Send(resp) == -1)
        fprintf(stderr, "failed to send data\n");
    }


  return 0;
}




/**
 * Start the server
 *
 * @return EXIT_FAILURE on error
 *
 * @details	This function starts the TCP Server,
 * 		then waits for a connection.  The server will
 * 		accept only one connection at a time.
 */
int main(void)
{
  t = new TCP_Server(SERVER_PORT);
  if (t->Start() == -1)
    {
      fprintf(stderr, "failed to start the server\n");
      return EXIT_FAILURE;
    }


  /* Keep handling incoming connections, one at a time */
  while (1)
    {
      /* Wait for an incoming connection */
      printf("Waiting for connection on TCP port %s...\n", t->port);

      if (t->wait_for_connection() == -1)
        fprintf(stderr, "failed to wait for a connection");

      else
        handle_connection();
    }


  t->KillServer();
  delete t;
  return 0;
}
