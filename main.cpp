#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include "cache.h"

bool parse_args(int &mem_size, int &cache_size, int &block_size,
                int &associativity, EVICTION &eviction_policy, int &init_policy,
                std::string &instructions_filename, int argc, char** argv) {
  for (int i=1; i < argc; i++) {
    void* arg_var;
    bool arg_is_int = true;
    if (strcmp(argv[i], "-m") == 0)
      arg_var = &mem_size;
    else if (strcmp(argv[i], "-c") == 0)
      arg_var = &cache_size;
    else if (strcmp(argv[i], "-b") == 0)
      arg_var = &block_size;
    else if (strcmp(argv[i], "-a") == 0)
      arg_var = &associativity;
    else if (strcmp(argv[i], "-e") == 0)
      arg_var = &eviction_policy;
    else if (strcmp(argv[i], "-i") == 0)
      arg_var = &init_policy;
    else if (strcmp(argv[i], "-f") == 0) {
      arg_var = &instructions_filename;
      arg_is_int = false;
    }
    else {
      std::cerr << argv[0] << ": " << argv[i] << ": No such option."
                << std::endl;
      return false;
    }

    if (++i == argc) {
      std::cerr << argv[0] << ": " << "Option " << argv[i-1] <<
        " requires an argument" << std::endl;
      return false;
    }
    if (arg_is_int) *(int*)arg_var = atoi(argv[i]);
    else
      *(std::string*)arg_var = argv[i];
  }
  if (instructions_filename.empty()) {
    std::cerr << argv[0] << ": Missing mandatory option -f" << std::endl;
    return false;
  }
  if (eviction_policy != LRU && eviction_policy != RANDOM) {
    std::cerr << argv[0] << ": Invalid eviction policy value." << std::endl;
    return false;
  }
  return true;
}

void run_program(std::ifstream& instructions, std::ofstream& stats,
                 std::ofstream& log, Cache& cache, Memory& mem) {
  std::string line;
  std::istringstream lineStream(std::ios::in);
  std::string command;
  int address, index = 0;
  int value;
  int loadHits = 0, storeHits = 0;
  int prevLoadHits = 0, prevStoreHits = 0;
  lineStream.setf(std::ios_base::hex | std::ios_base::internal |
                  std::ios_base::right, std::ios_base::basefield);
  stats.setf(std::ios_base::fixed, std::ios_base::floatfield);
  stats.precision(2);
  std::getline(instructions, line);
  while (not instructions.eof()) {
    value = 0;
    lineStream.str(line);
    lineStream.seekg(0);
    lineStream >> command >> address;

    log << index << " " << line << std::endl;
    if (command == "load") {
      value = cache.loadWord(address, mem);
      loadHits = cache.getLoadHits();
      log << index << " load ";
      if (loadHits == prevLoadHits)
        log << "miss ";
      else
        log << "hit ";
      log << std::hex << std::uppercase << value << std::endl;
    } else {
      lineStream >> value;
      cache.storeWord(address, mem, value);
      storeHits = cache.getStoreHits();
      log << index << " store ";
      if (storeHits == prevStoreHits)
        log << "miss";
      else
        log << "hit";
      log << std::endl;
    }
    prevLoadHits = loadHits;
    prevStoreHits = storeHits;
    index++;
    std::getline(instructions, line);
  }

  int loadMisses = cache.getLoadMisses();
  int storeMisses = cache.getStoreMisses();
  float loadMissRate = (float)loadMisses/(loadHits + loadMisses);
  float storeMissRate = (float)storeMisses/(storeHits + storeMisses);
  float totalMissRate = (float)(loadMisses + storeMisses)/
    (loadMisses + loadHits + storeMisses + storeHits);
  stats << "Instruction Count: " << index << std::endl;
  stats << "Load Hits: " << loadHits << std::endl;
  stats << "Load Misses: " << loadMisses << std::endl;
  stats << "Store Hits: " << storeHits << std::endl;
  stats << "Store Misses: " << storeMisses << std::endl;
  stats << "Load Miss Rate: " << loadMissRate*100 << "%" << std::endl;
  stats << "Store Miss Rate: " << storeMissRate*100 << "%" << std::endl;
  stats << "Total Miss Rate: " << totalMissRate*100 << "%" << std::endl;

}

int main(int argc, char** argv) {
  int mem_size = 1024, cache_size = 128, block_size = 4, associativity = 1,
    init_policy = 0;
  EVICTION eviction_policy;
  std::string instructions_filename;

  if (!parse_args(mem_size, cache_size, block_size, associativity,
                  eviction_policy, init_policy, instructions_filename, argc, argv))
    return 1;

  Cache cache(cache_size, block_size, associativity, eviction_policy);
  Memory mem(mem_size, init_policy, block_size);

  std::ifstream instructions_file;
  std::ofstream stats_file, log_file;
  
  instructions_file.open(instructions_filename, std::ios::in);
  if (!instructions_file.is_open()) {
    std::cerr << argv[0] << ": Can't open instructions file "
              << instructions_filename << std::endl;
    return 2;
  }
  stats_file.open("stats.log", std::ios::out | std::ios::trunc);
  if (!stats_file.is_open()) {
    std::cerr << argv[0] << ": Can't open stats.log" << std::endl;
    return 2;
  }
  log_file.open("inst.log", std::ios::out | std::ios::trunc);
  if (!log_file.is_open()) {
    std::cerr << argv[0] << ": Can't open inst.log" << std::endl;
    return 2;
  }

  run_program(instructions_file, stats_file, log_file, cache, mem);
  return 0;
}
