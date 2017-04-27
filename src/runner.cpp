#include <iostream>
#include <thread>

#include "logconfig.h"
#include "logger.h"

const int iterations = 1000;
void withdraw(int &account) {
  for (int i = 0; i < iterations; i++) {
    account--;
    LOG(WARNING) << "Balance after withdraw: " << account;
  }
}

void deposit(int &account) {
  for (int i = 0; i < iterations; i++) {
    account++;
    LOG(INFO) << logger::hash::off
              << "Balance after deposit: " << logger::hash::on << account;
  }
}

int main() {
  LOG.set_filter([](auto &l) {
    l.info.level += 30;
    return l.message.str().find("deposit") != std::string::npos;
  });
  LOG(DEBUG) << "Begin logging";
  int balance = 10000;

  std::thread withdrawer(withdraw, std::ref(balance));
  std::thread depositor(deposit, std::ref(balance));

  withdrawer.join();
  depositor.join();

  return 0;
}
