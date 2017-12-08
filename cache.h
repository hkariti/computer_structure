#include <iostream>
#include <exception>
#include <cmath>
#include <time.h>
#include <cstdlib>


typedef enum{
  RANDOM = 0,
    LRU = 1,
} EVICTION;

class Memory{
// private:
public:
  int memorySize;
  int block_size;
  char* data;

  char randomChar(){
    srand(time(NULL));
    int i = rand() % 256;
    return i;
  }

public:
  class randomExecption : std::exception {};

  Memory(int size, int start_val, int block_size){
    this->memorySize = size;
    this->block_size = block_size;
    this->data = new char[memorySize];
    for(int i = 0; i < memorySize; i++){
      if(start_val == 0){
        *(data + i) = 0xff;
      }
      else{
        *(data + i) = randomChar();
      }
    }
  }

  ~Memory(){
    delete[] data;
  }

  char* getBlock(int address){
    char* new_block = new char[block_size];
    int offset;
    if(address == 0){
	offset = 0;
    }
    else{
	offset = address % block_size;
    }
    for(int i = 0; i < block_size; i++){
      *(new_block + i) = *(data + address - offset + i);
    }
    return new_block;
  }

  void replaceWord(int address, char new_word){
    *(data + address) = new_word;
  }
};



//class that is one entry in the cache including metadata
class cacheEntry{
// private:
public:
  int LRUstanding; //most recently used = 0, least recently used = set_size - 1
  int valid;
  char* data;
  int tag;
  int block_size;

public:
  // prob wont be used.
  cacheEntry(int block_size, int LRUstanding, int valid){
    LRUstanding = LRUstanding;
    valid = valid;
    data = new char[block_size];
    tag = -1;
    block_size = block_size;
  }

  //make a new cache entry of size block_size that is empty
  cacheEntry(int block_size){
    this->block_size = block_size;
    LRUstanding = -1;
    valid = -1;
    tag = -1;
    data = new char[block_size];
  }

  ~cacheEntry(){
    delete[] data;
  }

  //get word from entry at a certian offset
  char getWord(int offset){
    return *(data + offset);
  }

  //get the LRU standing of the block
  int getLRU(){
    return LRUstanding;
  }

  //get the tag of the block
  int getTag(){
    return tag;
  }

  //get the valid of the block
  int getValid(){
    return valid;
  }

  //update all of the data of a certian entry, meant to be used for brining whole blocks from the memory to the cache
  void updateEntryData(char* new_data){
    for(int i = 0; i < block_size; i++){
      *(data + i) = *(new_data + i);
    }
  }

  // update the word at a certian offset in the entry, meant to be used for write hits in the cache.
  void updateEntryData(int offset, char new_data){
    *(data + offset) = new_data;
  }

  //updates the valid bit of a block
  void updateValid(int new_valid){
    valid = new_valid;
  }

  //updates the LRU standing of the block
  void updateEntryLRU(int new_LRU){
    LRUstanding = new_LRU;
  }

  void updateTag(int new_tag){
    tag = new_tag;
  }

};


//this class is a set of cacheEntries in the cache
class set{
// private:
public:
  int blocks_per_set;
  cacheEntry** data;
  EVICTION policy;

public:


  class randomExecption : std::exception {};
  //creats a new set with a specific amount of blocks of a specific size.
  set(int blocks_per_set, int block_size, EVICTION evic){
    this->blocks_per_set = blocks_per_set;
    this->policy = evic;
    this->data = new cacheEntry*[blocks_per_set];
    for ( int i = 0; i < this->blocks_per_set; i++){
      cacheEntry* temp = new cacheEntry(block_size);
      *(data + i) = temp;
    }
  }

  ~set(){
    for( int i = 0; i < blocks_per_set; i++){
      delete *(data + i);
    }
    delete[] data;
  }

  // find a valid entry with this tag in the set, if none exists - returns NULL
  cacheEntry* findEntryTag(int tag){
    for(int i = 0; i < blocks_per_set; i++){
      if((*(data + i))->getTag() == tag && (*(data + i))->getValid() == 1){
        return *(data + i);
      }
    }
    return NULL;
  }
  // updates the LRU standing inside the block so that the entry with this tag is the most recently used.
  void updateLRU(int tag){
    int prev_LRU = -2;
    for(int i = 0; i < blocks_per_set; i++){
      if((*(data + i))->getTag() == tag){
        prev_LRU = (*(data + i))->getLRU();
      }
    }
    for( int i = 0; i < blocks_per_set; i++){
	if((*(data + i))->getTag() == tag){
	  (*(data + i))->updateEntryLRU(0);
	}
	else{
	  if( (*(data + i))->getLRU() == -1){
		continue;
	  }
	  else{
		if((*(data + i))->getLRU() <= prev_LRU){
			int temp = (*(data + i))->getLRU();
			(*(data + i))->updateEntryLRU(temp + 1);
		}
	  }
	}
    }
  }

  // finds the block that should be evicted inside the set, the one that is least recently used = LRUstanding == blocks_per_set - 1
  int findEvicLRU(){
    for(int i = 0; i < blocks_per_set; i++){
      if((*(data + i))->getLRU() == (blocks_per_set-1)){
        return i;
      }
    }
  }

  int findEvicRandom(){
    std::srand(time(NULL));
    int i = std::rand() % blocks_per_set;
    return i; // FIXME how to randomize this??
  }

  void updateBlock(char* new_data, int tag){
    int i = -1;
    for( int j = 0; j < blocks_per_set; j++){
	if((*(data + j))->getValid() != 1){
		i = j;
		break;
	}
    }
    if( i == -1){
	 if(policy == LRU){
     		 i = findEvicLRU();
    	}
    	else{
      	i = findEvicRandom();
    	}
    }
    cacheEntry* entry = *(data + i);
    entry->updateEntryData(new_data);
    entry->updateTag(tag);
    entry->updateValid(1);
  }

};


class Cache{
public:
  int num_of_blocks;
  int blocks_per_set;
  int block_size; //in bytes(=chars)
  int cache_size; //in bytes(=chars)
  int num_of_sets; //associativity
  set** data;
  EVICTION policy;

  //statistics
  int load_hits;
  int load_misses;
  int store_hits;
  int store_misses;
  //int instruction_count;

public:

  // create a new cache
  Cache(int cache_size, int block_size, int associativity, int evic){
    this->cache_size = cache_size;
    this->block_size = block_size;
    this->num_of_blocks = cache_size / block_size;
    this->blocks_per_set = associativity;
    this->num_of_sets = num_of_blocks / blocks_per_set;
    if( evic == 0 ){
      this->policy = RANDOM;
    }
    else{
      this->policy = LRU;
    }

    this->data = new set*[num_of_sets];
    for (int i = 0; i < num_of_sets; i++){
      set* temp = new set(blocks_per_set, block_size, policy);
      *(data + i) = temp;
    }

    load_hits = 0;
    load_misses = 0;
    store_misses = 0;
    store_hits = 0;
  }

  ~Cache(){
    for(int i = 0; i < num_of_sets; i++){
      delete *(data + i);
    }
    delete[] data;
  }

  // should be invoked when servicing a load word command. Will update laod hit\miss stats as well as update the cache if it is a miss returns a single char that is the answer to the load request.
  char loadWord(int address, Memory& mem){
    int bits_index_and_offset = log(block_size) + log(num_of_sets);
    int index = address % (int)(pow(2, bits_index_and_offset));
    index = index / block_size;
    int offset = address % block_size;
    int tag = address / (pow(2, bits_index_and_offset));


    set* current = *(data + index);
    cacheEntry* entry = current->findEntryTag(tag);
    if(entry != NULL){
      load_hits++;
      current->updateLRU(tag);
      return entry->getWord(offset);
    }
    else{
      load_misses++;
      char* new_block = mem.getBlock(address);
      current->updateBlock(new_block, tag);
      current->updateLRU(tag);
      char result = *(new_block + offset);
      delete[] new_block;
      return result;
    }
  }


  // should be invoked when servicing a store word command
  void storeWord(int address, Memory& mem, char word){
    int bits_index_and_offset = log(block_size) + log(num_of_sets);
    int index = address % (int)(pow(2, bits_index_and_offset));
    index = index / block_size;
    int offset = address % block_size;
    int tag = address / (pow(2, bits_index_and_offset));

    set* current = *(data + index);
    cacheEntry* entry = current->findEntryTag(tag);
    if(entry != NULL){
      store_hits++;
      mem.replaceWord(address, word);
      entry->updateEntryData(offset, word);
      return;
    }
    else{
      store_misses++;
      mem.replaceWord(address, word);
      char* new_block = mem.getBlock(address);
      current->updateBlock(new_block, tag);
      current->updateLRU(tag);
      delete[] new_block;
      return;
    }

  }

  int getLoadHits(){
    return load_hits;
  }

  int getLoadMisses(){
    return load_misses;
  }

  int getStoreHits(){
    return store_hits;
  }

  int getStoreMisses(){
    return store_hits;
  }


};

