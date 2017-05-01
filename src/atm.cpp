#include <iostream>
#include <thread>
#include "logconfig.h"
#include "logger.h"
#include <unistd.h>

const int iterations = 1000;
void withdraw(int &account) {
  for (int i = 0; i < iterations; i++) {
    account--;
    LOG(WARNING) << "Balance after withdraw: " << account
                 << " logging a random integer " << 1000 + rand() % 100;
    // sleep proportional to (t-mid)^2
    usleep([](float x) { return (x * x) / 10; }(i - (iterations / 2)));
  }
}

void deposit(int &account) {
  for (int i = 0; i < 2 * iterations; i++) {
    const int delta = std::max((10001 - account) / 2, 0);
    LOG(DEBUG) << "Check";
    if (delta) {
      account += delta;
      LOG(INFO) << logger::hash::off
                << "Balance after deposit: " << logger::hash::on << account
                << " logging a random integer " << 1000 + rand() % 100;
    }
    // sleep for an average of n^2
    usleep(iterations * iterations / 10);
  }
}

int main() {

  LOG(DEBUG) << "Start program, begin logging";
  int balance = 10000;

  std::thread withdrawer(withdraw, std::ref(balance));
  std::thread depositor(deposit, std::ref(balance));

  withdrawer.join();
  depositor.join();

  LOG(DEBUG) << "End program, stop logging";

  return 0;
}
