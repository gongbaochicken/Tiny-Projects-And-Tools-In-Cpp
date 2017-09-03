#ifndef __CACHE_H__
#define __CACHE_H__

#include "cacheinstance.hpp"

#include <iostream>
#include <pthread.h>
#include <cassert>
#include <utility>
#include <unordered_map>
#include <list>

using namespace std;

class Cache
{
 private: 
  pthread_mutex_t mutex;  
  size_t cacheMaxSize;
  size_t cacheSize;
  unordered_map<string, CacheInstance*> memoryMap;
  list<string> memoryList;

  void erase(unordered_map<string, CacheInstance*>::iterator& ite);
  void erase(list<string>::iterator& ite);
  
 public:
  Cache();
  ~Cache();
  void setCacheSize(size_t size);
  CacheInstance* get(string& key);
  void put(const string& key, CacheInstance* CacheInstance);
};

#endif
