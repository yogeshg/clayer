#include "logger.h"


int main() {
  logger::Logger<logger::DEBUG, ST("mylog.txt")> l;
  l << "hey" << 2+2 << " what is up";
  return 0;
}

