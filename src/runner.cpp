#include <iostream>
#include <thread>
#include "stdlib.h"
#include "logconfig.h"
#include "logger.h"
#include <unistd.h>


const int iterations = 1000;
void withdraw(int &account) {
  for (int i = 0; i < iterations; i++) {
    account--;
    LOG(WARNING) << "Balance after withdraw: " << account
    << " logging a random integer " << 1000+rand()%100;
    usleep(i);
  }
}

void deposit(int &account) {
  for (int i = 0; i < iterations; i++) {
    account++;
    LOG(INFO) << logger::hash::off
              << "Balance after deposit: " << logger::hash::on << account
              << " logging a random integer " << 1000+rand()%100;
    usleep(iterations-i);
  }
}

int main() {
  LOG.set_filter([](auto &l) {
    l.info.level += 30;
    return true;
  });
  LOG(DEBUG) << "Begin logging";
  int balance = 10000;

  std::thread withdrawer(withdraw, std::ref(balance));
  std::thread depositor(deposit, std::ref(balance));

  withdrawer.join();
  depositor.join();

  return 0;
}
