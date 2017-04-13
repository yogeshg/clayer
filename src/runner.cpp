#include <iostream>
#include <thread>

#include "logconfig.h"
#include "logger.h"

const int iterations = 1000;
void withdraw(int &account){
  for(int i = 0; i < iterations; i++){
    account--;
    LOG(WARNING) << account;
  }
}

void deposit(int &account){
  for(int i = 0; i < iterations; i++){
    account++;
    LOG(INFO) << account;
  }
}

int main(){
  LOG(DEBUG) << "Begin logging";
  int balance = 10000;

  std::thread withdrawer(withdraw,std::ref(balance));
  std::thread depositor(deposit,std::ref(balance));

  withdrawer.join();
  depositor.join();

  return 0;
}
