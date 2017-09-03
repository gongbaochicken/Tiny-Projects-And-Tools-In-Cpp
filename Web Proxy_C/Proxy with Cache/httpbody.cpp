#include "httpbody.hpp"

HttpBody::HttpBody()
{

}

HttpBody::~HttpBody()
{

}
  
void HttpBody::append(char* buf, size_t size)
{
  body.append(buf, size);
}

string& HttpBody::getBody()
{
  return body;
}

size_t HttpBody::getBodySize()
{
  return body.size();
}
