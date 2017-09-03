#ifndef __HEADERCONTROL_H__
#define __HEADERCONTROL_H__

#include <cassert>
#include <string>
#include <cstring>

using namespace std;

class HttpHeader
{
 private:
  static const string connectionK;
  static const string contentLengthK;
  static const string cacheControlK;

  string header;

  bool persistent;
  bool cacheable;
  size_t bodySize;
  bool chunked;
  
 public:
  HttpHeader();
  ~HttpHeader();
  void append(char* buf, ssize_t size);
  string& getHeader();
  bool isPersistent();
  bool isCachable();
  size_t getHeaderSize();
  size_t getBodySize();
  bool isChunked();
  
};

#endif
