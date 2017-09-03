#ifndef __HTTPBODY_H__
#define __HTTPBODY_H__

#include <string>

using namespace std;

class HttpBody
{
 private:
  string body;
  
 public:
  HttpBody();
  ~HttpBody();
  
void append(char* buf, size_t size);
  string& getBody();
  size_t getBodySize();
};

#endif
