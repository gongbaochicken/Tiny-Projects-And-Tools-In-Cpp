#include "cacheinstance.hpp"

CacheInstance::CacheInstance()
{
  header = nullptr;
  body = nullptr;
}

CacheInstance::~CacheInstance()
{
  delete header;
  delete body;
}

void CacheInstance::setHeader(HttpHeader* header)
{
  this->header = header;
}

HttpHeader* CacheInstance::getHeader()
{
  return header;
}

void CacheInstance::setBody(HttpBody* body)
{
  this->body = body;
}

HttpBody* CacheInstance::getBody()
{
  return body;
}  

size_t CacheInstance::getTotSize()
{
  size_t headerSize = header->getHeaderSize();
  ssize_t bodySize = header->getBodySize();
  assert((bodySize == (ssize_t)body->getBodySize() ||
          bodySize == -1) &&
         "header's body size != body size");
  return headerSize+bodySize;
}

void CacheInstance::setIterator(list<string>::iterator ite)
{
  this->ite = ite;
}

list<string>::iterator& CacheInstance::getIterator()
{
  return ite;
}
