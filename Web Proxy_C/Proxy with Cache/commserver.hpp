#ifndef __COMMSERVER_H__
#define __COMMSERVER_H__

#include <iostream>
#include <cstdint>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define LISTENQUEUE 1024

using namespace std;

class CommServer
{
 private:
  int socket_fd;
  uint16_t port; // little endian
  struct sockaddr_in sockAddr;

 public:
  CommServer(uint16_t port);
  ~CommServer();

  bool openx();
  bool closex();
  int acceptx();
};

#endif
