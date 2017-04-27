#ifndef __LOGCONFIG_H__
#define __LOGCONFIG_H__

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

#include "logger.h"

namespace logger {
enum Severity {
  NOTSET = 0,
  DEBUG = 10,
  INFO = 20,
  WARNING = 30,
  ERROR = 40,
  CRITICAL = 50
};

void prop_time(std::ostream &o, const ContextInfo &i) {
  using std::chrono::system_clock;
  auto now = system_clock::to_time_t(system_clock::now());
  o << std::put_time(std::localtime(&now), "%F %T");
}

void prop_thread(std::ostream &o, const ContextInfo &i) {
  auto f = o.flags();
  o << std::hex << std::showbase << "Thread " << std::hex << std::showbase
    << std::this_thread::get_id();
  o.flags(f);
}

constexpr const char format[] = "\033[1;31m[%]\033[0m % %(%:%): [%]";
using MainLogger =
    Logger<DEBUG, std::ostream, std::clog, format,
           prop_time, prop_thread, prop_file, prop_func, prop_line, prop_msg>;

// Example extended logger
// void prop_ip(std::ostream &o, const ContextInfo &i) { o << "127.0.0.1"; }
// std::ofstream log_stream("log.txt");
// constexpr const char alt_fmt[] = "%(%:%): % [%]";
// using AltLogger = Logger<DEBUG, std::ofstream, log_stream, alt_fmt,
//                         prop_file, prop_func, prop_line, prop_msg, prop_ip>;

#define LOG(severity)                                                          \
  logger::MainLogger::getInstance().log<logger::severity>(                     \
      {__FILE__, __func__, __LINE__})
}

#endif /* __LOGCONFIG_H__ */
