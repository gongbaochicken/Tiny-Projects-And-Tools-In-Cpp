#ifndef __COMMCLIENT_H__
#define __COMMCLIENT_H__

#include <iostream>
#include <unistd.h>
#include <cstdint>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <algorithm>

#define MAXLINE 8192

using namespace std;

class CommClient
{
 private:
  int socket_fd;
  uint16_t port; // little endian
  string host;
  struct sockaddr_in sockaddr;
  
  char buffer[MAXLINE];
  char* buf_ptr;
  int32_t counter;

  void init(const char* host, uint16_t port);
  int32_t _read(char* buf, int32_t size);
  int32_t _read_fb(char* buf, int32_t size);
  int32_t _write(const char* buf, int32_t size);

 public:
  CommClient();
  CommClient(string host, uint16_t port);
  CommClient(char* host, uint16_t port);
  CommClient(int sockfd);
  ~CommClient();

  bool openx();
  bool closex();
  void setHostPort(const char* host, uint16_t port);
  int32_t readlinex(char* buf, int32_t size);
  int32_t readx(char* buf, int32_t size);
  int32_t writex(const char* buf, int32_t size);  
};

#endif
