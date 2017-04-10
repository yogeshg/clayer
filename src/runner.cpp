#include "logconfig.h"
#include "logger.h"

int main() {

  logger::Logger::getInstance().log<logger::DEBUG>() << "logging\n";
  logger::Logger::getInstance().log<logger::INFO>() << "logging";
  logger::Logger::getInstance().log<logger::WARNING>() << "logging";
  // logger::Logger2<100>::getInstance()->log() << "logging 3\n";

  return 0;
}

