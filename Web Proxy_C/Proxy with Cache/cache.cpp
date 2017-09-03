#include "cache.hpp"
  
Cache::Cache()
{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&mutex, &attr);
  pthread_mutexattr_destroy(&attr);
  cacheSize = 0;  
  cacheMaxSize = 0;  
}

Cache::~Cache()
{
  pthread_mutex_destroy(&mutex);
  cacheSize = 0;
  cacheMaxSize = 0;
  memoryMap.clear();
  memoryList.clear();
}

void Cache::setCacheSize(size_t size)
{
  cacheMaxSize = size;
}

void Cache::erase(unordered_map<string, CacheInstance*>::iterator& ite_map)
{
  pthread_mutex_lock(&mutex);
  cacheSize -= ite_map->second->getTotSize();
  memoryList.erase(ite_map->second->getIterator());
  memoryMap.erase(ite_map);
  pthread_mutex_unlock(&mutex);
}

void Cache::erase(list<string>::iterator& ite_list)
{
  pthread_mutex_lock(&mutex);
  auto ite_map = memoryMap.find(*ite_list);
  assert(ite_map != memoryMap.end() &&
         "cannot find ite_map in cache when erasing");
  
  cacheSize -= ite_map->second->getTotSize();
  memoryList.erase(ite_list);
  memoryMap.erase(ite_map);
  pthread_mutex_unlock(&mutex);
}

CacheInstance* Cache::get(string& key)
{
  pthread_mutex_lock(&mutex);
  auto ite_map = memoryMap.find(key);
  if(ite_map == memoryMap.end())
  {
    pthread_mutex_unlock(&mutex);
    return nullptr;
  }
  else
  {
    auto ite_list = ite_map->second->getIterator();
    memoryList.erase(ite_list);
    memoryList.push_back(key);
    ite_list = memoryList.end();
    --ite_list;
    ite_map->second->setIterator(ite_list);
    CacheInstance* result = ite_map->second;
    pthread_mutex_unlock(&mutex);
    return result;
  }
}

void Cache::put(const string& key, CacheInstance* CacheInstance)
{
  pthread_mutex_lock(&mutex);
  auto ite_map = memoryMap.find(key);
  if(ite_map != memoryMap.end())
    erase(ite_map);
  
  memoryMap.insert(make_pair(key, CacheInstance));
  ite_map = memoryMap.find(key);
  assert(ite_map != memoryMap.end());
  memoryList.push_back(key);
  auto ite_list = memoryList.end();
  --ite_list;
  ite_map->second->setIterator(ite_list);
  cacheSize += ite_map->second->getTotSize();

  while(cacheSize > cacheMaxSize)
  {
    ite_list = memoryList.begin();
    erase(ite_list);
  }
  
  pthread_mutex_unlock(&mutex);
}
