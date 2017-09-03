#ifndef __CACHEINSTANCE_H__
#define __CACHEINSTANCE_H__

#include "httpheader.hpp"
#include "httpbody.hpp"

#include <cassert>
#include <string>
#include <list>
#include <unordered_map>

using namespace std;

class CacheInstance
{
 private:
  HttpHeader* header;
  HttpBody* body;
  list<string>::iterator ite;
  
 public:
  CacheInstance();
  ~CacheInstance();

  void setHeader(HttpHeader* header);
  HttpHeader* getHeader();
  void setBody(HttpBody* body);
  HttpBody* getBody();

  size_t getTotSize();
  
  void setIterator(list<string>::iterator ite);
  list<string>::iterator& getIterator();
};

#endif

// cache control
// connection
// content length
