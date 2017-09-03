#include "commserver.hpp"

CommServer::CommServer(uint16_t port)
{
  this->socket_fd = -1;
  this->port = port;
  memset(&sockAddr, 0, sizeof(sockAddr));
}

CommServer::~CommServer()
{
  
}

bool CommServer::openx()
{
  if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    socket_fd = -1;
    return false;
  }

  int optval = 1;
  if(setsockopt(socket_fd,
                SOL_SOCKET,
                SO_REUSEADDR, 
                (const void *)&optval,
                sizeof(int)) < 0)
  {
    close(socket_fd);
    socket_fd = -1;
    return false;
  }

  memset(&sockAddr, 0, sizeof(sockAddr));
  sockAddr.sin_family = AF_INET; 
  sockAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  sockAddr.sin_port = htons(port);
  
  if(bind(socket_fd,
          (struct sockaddr*)&sockAddr,
          sizeof(sockAddr)) < 0)
  {
    close(socket_fd);
    socket_fd = -1;
    return false;
  }

  if(listen(socket_fd, LISTENQUEUE) < 0)
  {
    close(socket_fd);
    socket_fd = -1;
    return false;
  }
  else
    return true;
}

bool CommServer::closex()
{
  if(socket_fd < 0)
    return true;
  else
  {
    close(socket_fd);
    socket_fd = -1;
    return true;
  }  
}

int CommServer::acceptx()
{
  int new_fd;
  struct sockaddr_in new_sockAddr;
  int new_sockAddrLen = sizeof(new_sockAddr);
  if ((new_fd = accept(socket_fd,
                       (struct sockaddr*)&new_sockAddr,
                       (socklen_t*)&new_sockAddrLen)) < 0)
    perror("accept error\n");    
  return new_fd;
}
