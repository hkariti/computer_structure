
#include "cache.h"


void allocationTest(){
  std::cout << "starting allocation test...";
  Cache cache(4,1,4,1);
  if(cache.getLoadHits() != 0){
    std::cout << "FAILED" << std::endl;
    return;
  }
  if(cache.getLoadMisses() != 0){
    std::cout << "FAILED" << std::endl;
    return;
  }
  if(cache.getStoreHits() != 0){
    std::cout << "FAILED" << std::endl;
    return;
  }
  if(cache.getStoreMisses() != 0){
    std::cout << "FAILED" << std::endl;
    return;
  }

  if(cache.num_of_blocks != 4){
    std::cout << "FAILED" << std::endl;
    return;
  }
  if(cache.blocks_per_set != 4){
    std::cout << "FAILED" << std::endl;
    return;
  }
  if(cache.policy != LRU){
    std::cout << "FAILED" << std::endl;
    return;
  }
  std::cout << "PASSED" << std::endl;
  return;
}

void memAllocationTest(){
 Memory mem(16, 0, 1);
}

void storeWordTest(){
  std::cout << "starting the store word test...";
  Cache cache(8,2,2,1);
  Memory mem(32, 0, 2);
  cache.storeWord(0, mem, 'a');
  if(*(mem.data) != 'a'){
    std::cout << "FAILED" << std::endl;
    return;
  }
  set* currentSet = *(cache.data);
  cacheEntry* entry = *(currentSet->data);
  if(*(entry->data) != 'a'){
    std::cout << "FAILED" << std::endl;
    return;
  }
  if(entry->getValid() != 1){
	std::cout << "FAILED (entry not set to valid)" << std::endl;
	return;
  }
  if(entry->getLRU() != 0){
	std::cout << "FAILED (entry LRU not correct valid)" << std::endl;
	return;
  }
  if(cache.load_hits != 0 || cache.load_misses != 0 || cache.store_hits != 0 || cache.store_misses != 1){
    std::cout << "FAILED (first store, cache miss)" << std::endl;
    return;
  }

  cache.storeWord(0, mem, 'a');
  if(*(mem.data) != 'a'){
    std::cout << "FAILED" << std::endl;
    return;
  }
  currentSet = *(cache.data);
  entry = *(currentSet->data);
  if(*(entry->data) != 'a'){
    std::cout << "FAILED" << std::endl;
    return;
  }
  if(cache.load_hits != 0 || cache.load_misses != 0 || cache.store_hits != 1 || cache.store_misses != 1){
    std::cout << "FAILED (second store, cache hit)" << std::endl;
    return;
  } 

  cache.storeWord(5, mem, 'b');
  if(cache.load_hits != 0 || cache.load_misses != 0 || cache.store_hits != 1 || cache.store_misses != 2){
    std::cout << "FAILED (third store, cache miss)" << std::endl;
    return;
  }

  cache.storeWord(5, mem, 'c');
  if(cache.load_hits != 0 || cache.load_misses != 0 || cache.store_hits != 2 || cache.store_misses != 2){
    std::cout << "FAILED (fourth store, cache hit)" << std::endl;
    return;
  }

  cache.storeWord(10, mem, 'd');
  if(cache.load_hits != 0 || cache.load_misses != 0 || cache.store_hits != 2 || cache.store_misses != 3){
    std::cout << "FAILED (fifth store, cache miss)" << std::endl;
    return;
  }

  cache.storeWord(0, mem, 'd');
  if(cache.load_hits != 0 || cache.load_misses != 0 || cache.store_hits != 2 || cache.store_misses != 4){
    std::cout << "FAILED (sixth store, cache miss)" << std::endl;
    return;
  }

  if( cache.loadWord(0, mem) != 'd'){
	std::cout << "FAILED (first load, data hit)" << std::endl;
	return;
  }
  if(cache.load_hits != 1 || cache.load_misses != 0 || cache.store_hits != 2 || cache.store_misses != 4){
    std::cout << "FAILED (first load, cache hit)" << std::endl;
    return;
  }

  
  if ( cache.loadWord(5, mem) != 'c'){
	std::cout << "FAILED (second load, data miss " << cache.loadWord(5, mem) << ")" << std::endl;
	return;
  }
 if(cache.load_hits != 1 || cache.load_misses != 1 || cache.store_hits != 2 || cache.store_misses != 4){
    std::cout << "FAILED (second load, cache miss)" << std::endl;
    return;
  }

if( cache.loadWord(10, mem) != 'd'){
	std::cout << "FAILED (third load, data miss)" << std::endl;
	return;
}
  if(cache.load_hits != 1 || cache.load_misses != 2 || cache.store_hits != 2 || cache.store_misses != 4){
    std::cout << "FAILED (third store, cache miss)" << std::endl;
    return;
  }

  std::cout << "PASSED" << std::endl;
  return;
}



int main(){
  allocationTest();
memAllocationTest(); 
 storeWordTest();
  return 0;
}
