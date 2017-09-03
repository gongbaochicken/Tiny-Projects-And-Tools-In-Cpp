#include "httpheader.hpp"

const string HttpHeader::connectionK = "Connection:";
const string HttpHeader::contentLengthK = "Content-Length:";
const string HttpHeader::cacheControlK = "Cache-Control:";

HttpHeader::HttpHeader()
{
  persistent = true;
  cacheable = true;
  bodySize = 0;
  chunked = false;
}

HttpHeader::~HttpHeader()
{
  
}

void HttpHeader::append(char* buf, ssize_t size)
{
  if(strcasestr(buf, connectionK.c_str()) != nullptr)
  {
    char* value = buf+connectionK.size();
    if(strcasestr(value, "close") != nullptr)
      persistent = false;
    else if(strcasestr(value, "keep-alive") != nullptr)
      persistent = true;
    else
      return;
    header.append(buf, size);
  }
  else if(strcasestr(buf, contentLengthK.c_str()) != nullptr)
  {
    header.append(buf, size);
    char* value = buf+contentLengthK.size();    
    bodySize = atoi(value);
  }
  else if(strcasestr(buf, "Transfer-Encoding:") != nullptr)
  {
    header.append(buf, size);
    char* value = buf+strlen("Transfer-Encoding:");
    if(strcasestr(value, "chunked") != nullptr)
      chunked = true;
  }
  else if(strcasestr(buf, cacheControlK.c_str()) != nullptr)
  {
    char* value = buf+cacheControlK.size();
    if(strcasestr(value, "no-cache") != nullptr)
      cacheable = false;
    else if(strcasestr(value, "no-store") != nullptr)
      cacheable = false;
    else if(strcasestr(value, "private") != nullptr)
      cacheable = false;
    else if(strcasestr(value, "public") != nullptr)
      cacheable = true;
    else
      return;
    header.append(buf, size);
  }
  else
  {
    header.append(buf, size);
  }
}

string& HttpHeader::getHeader()
{
  return header;
}

bool HttpHeader::isPersistent()
{
  return persistent;
}

bool HttpHeader::isCachable()
{
  return cacheable;
}

size_t HttpHeader::getHeaderSize()
{
  return header.size();
}

size_t HttpHeader::getBodySize()
{
  return bodySize;
}


bool HttpHeader::isChunked()
{
  return chunked;
}
