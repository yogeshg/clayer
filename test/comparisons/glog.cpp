
#include <thread>
#include <vector>
#include <utility>
#include <glog/logging.h>
#include "stdlib.h"


const int iterations = 1000;
const int thread_count = 50;
void worker() {
  for (int i = 0; i < iterations; i++) {
    LOG(WARNING) << "ERROR message";
    LOG(INFO) << "INFO message";
    LOG(INFO) << "INFO message";
    LOG(ERROR) << "CRITICAL message";
    LOG(ERROR) << "ERROR message";
  }
}


int main(int argc, char **argv) {

  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Begin the performance test";
  std::vector<std::thread> worker_threads;
  for(int i = 0; i < thread_count; i++){
    worker_threads.push_back(std::move(std::thread(worker)));
  }

  for(auto &thread : worker_threads){
    thread.join();
  }

  return 0;
}
