#ifndef __LOGCONFIG_H__
#define __LOGCONFIG_H__

#include <regex>

#include "logger.h"

namespace logger {
// using MyFormat = Format(date, line, ip);

// Format("(%s:%s:%s)", date, line, ip);
enum Severity {
  NOTSET = 0,
  DEBUG = 10,
  INFO = 20,
  WARNING = 30,
  ERROR = 40,
  CRITICAL = 50
};

constexpr const char format[] = "%(%:%):";
using MainLogger = Logger<NOTSET, format, FILE, FUNC, LINE>;

// * Log record : each line of the log
// * log format : tuple like <Date, Name, Message>
// * log property : Date, Name, Message
#define LOG(severity)                                                          \
  logger::MainLogger::getInstance().log<logger::severity>(                     \
      {__FILE__, __func__, __LINE__})
}

#endif /* __LOGCONFIG_H__ */
