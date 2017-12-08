
#include "cache.h"


bool allocationTest(){
  Cache cache(4,1,4,1);
  if(cache.getLoadHits() != 0){
    return false;
  }
  if(cache.getLoadMisses() != 0){
    return false;
  }
  if(cache.getStoreHits() != 0){
    return false;
  }
  if(cache.getStoreMisses() != 0){
    return false;
  }

  if(cache.num_of_blocks != 4){
    return false;
  }
  if(cache.blocks_per_set != 1){
    return false;
  }
  if(cache.policy != LRU){
    return false;
  }

  return true;
}

int main(){
  if(!allocationTest()){
    std::cout << "failed allocation test :(";
  }
  return 0;
}
