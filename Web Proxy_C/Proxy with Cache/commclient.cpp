#include "commclient.hpp"


#include <time.h>

CommClient::CommClient()
{
  this->socket_fd = -1;
  memset(&sockaddr, 0, sizeof(sockaddr));
  this->port = 0;
  this->host.clear();

  memset(buffer, 0, MAXLINE);
  buf_ptr = buffer;
  counter = 0;
}

CommClient::CommClient(string host, uint16_t port)
{
  init(host.c_str(), port);
}

CommClient::CommClient(char* host, uint16_t port)
{
  init(host, port);
}

CommClient::CommClient(int sockfd)
{
  this->socket_fd = sockfd;
  memset(&sockaddr, 0, sizeof(sockaddr));
  this->port = 0;
  this->host.clear();

  memset(buffer, 0, MAXLINE);
  buf_ptr = buffer;
  counter = 0;
}

CommClient::~CommClient()
{
  
}

bool CommClient::openx()
{
  if(socket_fd > -1)
    return true;
  cout<< "open new socket to ";
  cout<<host<<" "<<port<<endl;
  cout<<"++++++++++++++++++++++++++"<<endl;
  if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return false;
  struct hostent *hp;
  if((hp = gethostbyname(host.c_str())) == NULL)
  {
    close(socket_fd);
    socket_fd = -1;
    return false;
  }

  // assign the value of sockaddr
  sockaddr.sin_family = AF_INET;
  memcpy((char *)&sockaddr.sin_addr.s_addr,
         (char *)hp->h_addr, 
         hp->h_length);
  sockaddr.sin_port = htons(port);

  // connect
  if((connect(socket_fd,
              (struct sockaddr*)&sockaddr,
              sizeof(sockaddr))) < 0)
  {
    close(socket_fd);
    socket_fd = -1;    
    return false;    
  }
  return true;
}

bool CommClient::closex()
{
  if(socket_fd < 0)
    return true;

  cout<< "close socket to ";
  cout<<host<<" "<<port<<endl;
  cout<<"----------------------------"<<endl;
  
  close(socket_fd);
  socket_fd = -1;    
  return true;    
}

void CommClient::init(const char* host, uint16_t port)
{
  this->socket_fd = -1;  
  memset(&sockaddr, 0, sizeof(sockaddr));
  this->port = port; // little endian
  this->host = string(host);

  memset(buffer, 0, MAXLINE);
  buf_ptr = buffer;
  counter = 0;
}

void CommClient::setHostPort(const char* host, uint16_t port)
{
  if(this->socket_fd >= 0)
  {
    if(this->host.compare(host) == 0 && this->port == port)
    {
      // do nothing
    }
    else
    {
      this->closex();
      init(host, port);
    }    
  }
  else
    init(host, port);
}

int32_t CommClient::_read(char* buf, int32_t size)
{
  if(socket_fd < 0)
    return -1;
  
  int32_t num_read;
  while((num_read = read(socket_fd, buf, size)) < 0)
  {
    if(errno == EINTR)
      continue;
    else
      return -1;
  }
  return num_read;
}

int32_t CommClient::_read_fb(char* buf, int32_t size)
{
  if(socket_fd < 0)
    return -1;

  if(counter <= 0)
  {
    // refill
    counter = _read(buffer, sizeof(buffer));
    buf_ptr = buffer;
    if(counter < 0)
    {
      counter = 0;
      return -1;
    }
    else if(counter == 0)
      return 0;
  }

  int32_t num_read = min(counter, size);
  memcpy(buf, buf_ptr, num_read);  
  counter -= num_read;
  buf_ptr += num_read;
  return num_read;  
}



int32_t CommClient::_write(const char* buf, int32_t size)
{
  if(socket_fd < 0)
    return -1;
  
  int32_t num_write;
  while((num_write = write(socket_fd, buf, size)) < 0)
  {
    if(errno == EINTR)
      continue;
    else
      return -1;
  }
  return num_write;
}

int32_t CommClient::readlinex(char* buf, int32_t size)
{
  int32_t num_remain = size;
  char c;
  while(num_remain > 1)
  {
    int32_t num_read = _read_fb(&c, 1);
    if(num_read == 0)
      break;
    else if(num_read < 0)
      return -1;
    else // num_read == 1
    {
      *buf = c;     
      ++buf;
      --num_remain;        
      if(c == '\n')
        break;
    }    
  }
  *buf = 0;
  return size-num_remain;
}

int32_t CommClient::readx(char* buf, int32_t size)
{
  if(counter > 0)
  {
    int32_t num_read = min(counter, size);
    memcpy(buf, buf_ptr, num_read);
    counter -= num_read;
    return num_read;
  }
  else if(size > 0)
  {
    int32_t num_read;
    if((num_read = _read(buf, size)) < 0)
      return -1;
    else
      return num_read;
  }
  else
    return 0;
}

int32_t CommClient::writex(const char* buf, int32_t size)
{
  int32_t num_remain = size;
  int32_t num_write;
  while(num_remain > 0)
  {
    if((num_write = _write(buf, size)) < 0)
      return -1;
    else
    {
      num_remain -= num_write;
      buf += num_write;
    }
  }
  return size;
}
