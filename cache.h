#include <iostream>
#include <exception>
#include <cmath>
typedef {
  RANDOM = 0,
    LRU = 1,
} EVICTION;


class cacheEntry{
private:
  int LRUstanding; //least recentry used = 0
  int valid;
  char* data;
  int tag;
  int block_size;

public:
  cacheEntry(int block_size, int LRUstanding, int valid){
    LRUstanding = LRUstanding;
    valid = valid;
    data = new char[block_size];
    tag = -1;
    block_size = block_size;
  }

  cacheEntry(int block_size){
    LRUstanding = -1;
    valid = -1;
    tag = -1;
    data = new char[block_size];
  }

  ~cacheEntry(){
    delete data;
  }

  char getWord(int offset){
    return *(data + offset);
  }

  int getLRU(){
    return LRUstanding;
  }

  int getTag(){
    return tag;
  }

  int getValid(){
    return valid;
  }

  void updateEntryData(char* new_data){
    for(int i = 0; i < block_size; i++){
      *(data + i) = *(new_data + i);
    }
  }

  void updateValid(int new_valid){
    valid = new_valid;
  }

  void updateEntryLRU(int new_LRU){
    LRUstanding = new_LRU;
  }

};

class set{
private:
  int blocks_per_set;
  cacheEntry** data;

public:
  set(int blocks_per_set, int block_size){
    num_of_blocks = num_of_blocks;
    data = new cacheEntry*[num_of_blocks];
    for ( int i = 0; i < blocks_per_set; i++){
      cacheEntry* temp = new cacheEntry(block_size);
      *(data + i) = temp;
    }
  }

  ~set(){
    for( int i = 0; i < block_size; i++){
      delete *(data + i);
    }
    delete data;
  }

  cacheEntry* findEntryTag(int tag){
    for(int i = 0; i < blocks_per_set; i++){
      if(*(data + i)->getTag() == tag && *(data + i)->getValid() == 1){
        return *(data + i);
      }
    }
    return NULL;
  }

  void updateLRU(int tag){
    int prev_LRU;
    for(int i = 0; i < blocks_per_set; i++){
      if(*(data + i)->getTag() == tag){
        prev_LRU = *(data + i)->getLRU();
      }
    }
    for( int i = 0; i < blocks_per_set; i++){
      if(*(data + i)->getLRU() <= prev_LRU){
        int temp = *(data + i)->getLRU();
        if(temp == prev_LRU){
          *(data + i)->updateLRU(0);
        }
        else{
          *(data + i)->updateLRU(temp + 1);
        }
      }
    }
  }

  int findEvic(){
    for(int i = 0; i < blocks_per_set; i++){
      if(*(data + i)->getLRU == 0){
        return i;
      }
    }
  }

  void updateBlock(char* new_data){
    int i = findEvic();
    cacheEntry* entry = *(data + i);
    entry->updateEntryData(new_data);
    entry->updateValid(1);
  }

  
};


class Cache{
private:
  int num_of_blocks;
  int blocks_per_set;
  int block_size;
  int cache_size;
  int num_of_sets; //associativity
  set** data;
  EVICTION policy;

  //statistics
  int load_hits;
  int load_misses;
  int store_hits;
  int store_misses;
  int instruction_count;

public:

  Cache(int cache_size, int block_size, int associativity, int evic){
    cache_size = cache_size;
    block_size = block_size;
    num_of_blocks = cache_size / block_size;
    num_of_sets = associativity;
    blocks_per_set = num_of_blocks / num_of_sets;
    if( evic == 0 ){
      policy = RANDOM;
    }
    else{
      policy = LRU;
    }

    data = new set*[num_of_sets];
    for (int i = 0; i < num_of_sets; i++){
      set temp = new set(blocks_per_set, block_size);
      *(data + i) = temp;
    }

    load_hits = 0;
    load_misses = 0;
    store_misses = 0;
    store_hits = 0;
    instruction_count = 0;
  }

  ~Cache(){
    for(int i = 0; i < num_of_sets; i++){
      delete *(data + i);
    }
    delete data;
  }

  char loadWord(int address, Memory mem){
    int bits_index_and_offset = log(block_size) + log(associativity);
    int index = address % (pow(2, bits_index_and_offset));
    int offset = address % block_size;
    int tag ; //FIXME


    set current = *(data + index);
    cacheEntry* entry = current->findEntryTag(tag);
    if(entry != NULL){
      load_hits++;
      current->updateLRU(tag);
      return entry->getWord(offset);
    }
    else{
      load_misses++;
      char* new_block = mem->getBlock(address);
      current->updateBlock(new_block);
      current->updateLRU(tag);
      char result = *(new_block + offset);
      delete new_block;
      return result;
    }
  }

};


class Memory{
private:
  int memorySize;
  int block_size;
  char* data;

  char randomChar(){
    return 'a';
  }

public:
  Memory(int size, int start_val, int block_size){
    memorySize = size;
    int block_size;
    data = new char[memorySize];
    for(int i = 0; i < memorySize; i++){
      if(start_val == 0){
        *(data + i) = 0xff;
      }
      else{
        *(data + i) = randomChar();
      }
    }
  }

  char* getBlock(int address){
    char* new_block = new char[block_size];
    int offset = address % block_size;
    for(int i = 0; i < block_size; i++){
      *(new_block + i) = *(data + address - offset + i);
    }
    return new_block;
  }

  void replaceWord(int address, char new_word){
    *(data + address) = new_word;
  }
};


