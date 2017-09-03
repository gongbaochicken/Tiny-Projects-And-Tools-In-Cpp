/*
Wenquan Xing, Zhuo Jia
*/

#include "commserver.hpp"
#include "commclient.hpp"
#include "httpheader.hpp"
#include "httpbody.hpp"
#include "cache.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>

#define CACHESIZE 104857600
#define debug 0

void* forward(void* args);
void* backward(void* args);
void* minion(void* args);
void secure(int clientfd, int serverfd);
bool parseAddress(char* url, string& command, string& host, uint16_t& port, string& file, string& version);

bool alive;
Cache cache;

int main(int argc, char** argv)
{
  alive = true;
  if (argc < 3 || argc > 3)
  {
    fprintf(stdout, "Command: ./%s [lintening port]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  uint16_t port = atoi(argv[1]);
  size_t cacheSize = 1024*atoi(argv[2]);
  
  sigset_t sig_pipe; 
  if(sigemptyset(&sig_pipe) || sigaddset(&sig_pipe, SIGPIPE))
    perror("creating sig_pipe set failed");
  if(sigprocmask(SIG_BLOCK, &sig_pipe, NULL) == -1)
    perror("sigprocmask failed");

  CommServer commServer(port);
  if(commServer.openx() == false)
  {
    perror("opening server error\n");
    exit(EXIT_FAILURE);
  }

  cache.setCacheSize(cacheSize);
  
  while(alive)
  {
    int* fd = new int;
    pthread_t session;
    *fd = commServer.acceptx();    
    pthread_create(&session, NULL, minion, (void*)fd);
    pthread_detach(session);
  }
  commServer.closex();  
  return 0;
}

bool parseAddress(char* url, string& command, string& host, uint16_t& port, string& file, string& version)
{
  const char* token = " \r\n";
  char* saveptr = NULL;
  char* hpf = NULL;
  char* colon = NULL;
  char* ptr = NULL;

  ptr = strtok_r(url, token, &saveptr);
  if(ptr == NULL)
    return false;
  command = string(ptr);

  hpf = strtok_r(NULL, token, &saveptr);
  if(hpf == NULL)
    return false;
  
  ptr = strtok_r(NULL, token, &saveptr);
  if(ptr == NULL)
    return false;
  version = string(ptr);

  port = command.compare("CONNECT") == 0 ? 443 : 80;

  if(strstr(hpf, "http://"))
    hpf += 7;
  else if(strstr(hpf, "https://"))
    hpf += 8;

  ptr = strchr(hpf, '/');
  file = string(ptr == NULL ? "/" : ptr);
  
  strtok_r(hpf, "/", &saveptr);
  if((colon = strchr(hpf, ':')) != NULL) 
  {
    *colon = 0;
    host = string(hpf);
    port = atoi(colon+1);
    return true;
  }
  else
  {
    host = string(hpf);
    return true;
  }	
}


void *minion(void* args)
{  
  int* fd = (int*)args;
  CommClient client(*fd);
  CommClient server;
  delete fd;
  args = nullptr;
  fd = nullptr;

  char buf[MAXLINE];
  char url[MAXLINE];
  string command;
  string host;
  string file;
  string version;
  uint16_t port;
  int32_t numBytes;

  string request;
  string header;
  string body;

  bool serverClose = false;
  bool clientClose = false;
  while(1)
  {
    if((numBytes = client.readlinex(buf, MAXLINE)) <= 0)
    {
      cout<<buf<<endl;
      client.closex();
      server.closex();
      return nullptr;
    }
    buf[MAXLINE-1] = 0;
    memcpy(url, buf, strlen(buf));
    if(parseAddress(url, command, host, port, file, version) == false)
    {
      client.closex();
      server.closex();
      return NULL;
    }

    if(command.compare("CONNECT") == 0)
    {
      client.closex();
      server.closex();
      return NULL;
    }
    request = command+" "+file+" "+version+"\r\n";

    if(debug)
    {
      cout<<"original one is\n"<<buf<<endl<<endl;
      // cout<<request;
      // cout<<host<<endl;
      // cout<<port<<endl;
    }
    
    HttpHeader clientHeader;
    while((numBytes = client.readlinex(buf, MAXLINE)) > 0)
    {
      clientHeader.append(buf, numBytes);      
      if(strcmp(buf, "\r\n") == 0)
        break;
    }
    
    CacheInstance* cacheInstance = NULL;
    clientClose = !clientHeader.isPersistent();
    size_t clientBodySize = clientHeader.getBodySize();
    string hf = host+file;
    // cout<<"cacheable ? "<<clientHeader.isCachable()<<endl;
    // cout<<"pointer is "<<cache.get(hf)<<endl;
    
    if(clientBodySize <= 0 &&
       clientHeader.isCachable() == true &&
       (cacheInstance = cache.get(hf)) != nullptr)
    {
      // send header&body to server
      // get header from the server
      // parse server's header
      // cache if possible
      // transfer the body and add body to cache

      cout<<"-----------------Cache hit!!!-----------------\n"<<endl;
      
      if(client.writex(
             cacheInstance->getHeader()->getHeader().c_str(),
             cacheInstance->getHeader()->getHeader().size()) == false)
      {
        client.closex();
        server.closex();
        return nullptr;
      }

      if(client.writex(
             cacheInstance->getBody()->getBody().c_str(),
             cacheInstance->getBody()->getBody().size()) == false)
      {
        client.closex();
        server.closex();
        return nullptr;
      }
    }
    else
    {
      // send header to server and get respond
      server.setHostPort(host.c_str(), port);
      if(server.openx() == false)
      {
        client.closex();
        server.closex();
        return nullptr;
      }
      
      server.writex(request.c_str(), request.size());
      server.writex(clientHeader.getHeader().c_str(),
                    clientHeader.getHeader().size());

      size_t clientBodySize = clientHeader.getBodySize();
      if(clientBodySize > 0)
      {
        if((numBytes = client.readx(buf, MAXLINE-1)) < 0)
        {
          client.closex();
          server.closex();
          return nullptr;
        }
        else if(numBytes == 0)
        {
          clientClose = true;
        }
        
        clientBodySize -= numBytes;
        
        if((numBytes = server.writex(buf, numBytes)) < 0)
        {
          client.closex();
          server.closex();
          return nullptr;
        }
      }
      
      if(debug)
      {
        // cout<<"WRITE TO SERVER"<<endl;
        cout<<request;
        cout<<clientHeader.getHeader();
        // cout<<"WRITE END"<<endl;
      } 
      
      HttpHeader* serverHeader = new HttpHeader();
      HttpBody* serverBody = new HttpBody();

      char resp[MAXLINE];
      char* saveptr;
      if((numBytes = server.readlinex(buf, MAXLINE)) <= 0)
      {        
        // cout<<buf<<endl;
        client.closex();
        server.closex();
        // cout<<"---------------END SERVER--------------"<<endl;
        return nullptr;
      }
      serverHeader->append(buf, numBytes);
      
      buf[MAXLINE-1] = 0;
      memcpy(resp, buf, strlen(buf));

      strtok_r(resp, " ", &saveptr);
      char* code = strtok_r(NULL, " ", &saveptr);
      assert(code != nullptr);
      int codeNum = atoi(code);        
      
      while((numBytes = server.readlinex(buf, MAXLINE)) > 0)
      {
        serverHeader->append(buf, numBytes);      
        if(strcmp(buf, "\r\n") == 0)
          break;
      }
      if(numBytes == 0)
      {
        serverClose = true;
      }
      else if(numBytes < 0)
      {
        client.closex();
        server.closex();
        return nullptr;
      }
      
      size_t serverBodySize = serverHeader->getBodySize();

      if(debug)
      {
        // cout<<"SERVER's HEADER"<<endl;
        cout<<serverHeader->getHeader();
        // cout<<"content length is "<<bodySize<<endl;
        // cout<<"HEADER END"<<endl;
      } 

      if((numBytes =
          client.writex(serverHeader->getHeader().c_str(),
                        serverHeader->getHeader().size())) < 0)
      {
        client.closex();
        server.closex();
        return nullptr;
      }
      if(serverHeader->isChunked())
      {
        clientClose = true;
        serverClose = true;
      }
      
      while(serverBodySize > 0 || serverHeader->isChunked())
      {
        if(serverBodySize > 0)
        {
          if((numBytes = server.readx(buf, MAXLINE-1)) < 0)
          {
            client.closex();
            server.closex();
            return nullptr;
          }
          else if(numBytes == 0)
          {
            serverClose = true;
            break;
          }
        }
        else if(serverHeader->isChunked())
        {
          if((numBytes = server.readlinex(buf, MAXLINE)) <= 0)
          {
            client.closex();
            server.closex();
            return nullptr;
          }
        }
        else
          cout<<"error\n";                

        if(serverBodySize > 0)          
          serverBodySize -= numBytes;
        
        if(serverHeader->isCachable())
          serverBody->append(buf, numBytes);
        
        if(debug)
        {          
          buf[numBytes] = 0;
          fprintf(stdout, "%s", buf);
          cout.flush();
        }
        
        if((numBytes = client.writex(buf, numBytes)) < 0)
        {
          client.closex();
          server.closex();
          // cout<<"@@@----"<<endl;
          return nullptr;
        }
        if(serverHeader->isChunked() && strcmp(buf, "\r\n") == 0)
           break;
      }
      
      serverClose = !serverHeader->isPersistent();
      if(serverHeader->isCachable() && codeNum == 200)
      {
        if(serverHeader->getBodySize() > 0 &&
           serverHeader->getBodySize() != serverBody->getBodySize())
        {
          cout<<"a haaaaaaaaaaaaaaaaaaaaaaaaaa\n";
          delete serverHeader;
          delete serverBody;
        }
        else if(serverHeader->isChunked() == false)
        {
          cacheInstance = new CacheInstance();
          cacheInstance->setHeader(serverHeader);
          cacheInstance->setBody(serverBody);    
          cache.put(host+file, cacheInstance);
        }
        else
        {
          delete serverHeader;
          delete serverBody;
        }
      }
      else
      {
        delete serverHeader;
        delete serverBody;
      }
      serverHeader = NULL;
      serverBody = NULL;

      if(clientClose == true)
      {
        client.closex();
        server.closex();
        // cout<<"@@@"<<endl;
        return nullptr;
      }
      else if(serverClose == true)
      {
        
        client.closex();
        server.closex();
        return nullptr;        
      }
    }
  }
  // return nullptr;
}
