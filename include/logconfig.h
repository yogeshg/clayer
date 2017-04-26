#ifndef __LOGCONFIG_H__
#define __LOGCONFIG_H__

#include <regex>

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

constexpr const char format[] = "%(%:%):";
using MainLogger = Logger<DEBUG, format, prop_file, prop_func, prop_line>;

#define LOG(severity)                                                          \
  logger::MainLogger::getInstance().log<logger::severity>(                     \
      {__FILE__, __func__, __LINE__})
}

#endif /* __LOGCONFIG_H__ */
