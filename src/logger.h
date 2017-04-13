#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <memory>
#include <type_traits>
#include <mutex>
// TODO: how do you specify defaults? add a logconfig.h to the library

// * Log record : each line of the log
// * log format : tuple like <Date, Name, Message>
// * log entity : Date, Name, Message
#define LOG(severity)                                                          \
  logger::Logger::getInstance().log<logger::severity>(__func__, __FILE__,      \
                                                      __LINE__)

namespace logger {

// TODO: how to specify THRESHOLD in terms of these? make two header files? NO!
enum Severity {
  NOTSET = 0,
  DEBUG = 10,
  INFO = 20,
  WARNING = 30,
  ERROR = 40,
  CRITICAL = 50
};

class Format {
  std::ostream &stream;
  const char *fn, *file; int line;
  std::lock_guard<std::mutex> lock;
public:
  // TODO: refrain from passing in 3 things, rather just 1 thing that has all
  // the info we need
  Format(std::ostream &s, const char *Fn, const char *File,
                  int Line, std::mutex &Logging_lock)
      : stream(s), fn(Fn), file(File), line(Line), lock(Logging_lock) {
    stream << file << ":" << line << "(" << fn << ")" << ": ";
  }
  ~Format() { stream << std::endl; }

  template <typename T> Format &operator<<(const T &s) {
    stream << s;
    return *this;
  }
};

class NoFormat {
public:
  template <typename T> NoFormat &operator<<(const T &s) { return *this; }
};

class Logger {
private:
  std::mutex logging_lock;
  Logger(){};

public:
  static Logger &getInstance() {
    static Logger instance;
    return instance;
  }

  template <unsigned int N,
            typename std::enable_if<N >= THRESHOLD>::type * = nullptr>
  Format log(const char *fn, const char *file, int line) {
    return {std::clog, fn, file, line, (logging_lock)};
  }

  template <unsigned int N,
            typename std::enable_if<N<THRESHOLD>::type * = nullptr> NoFormat
                log(const char *fn, const char *file, int line) {
    return {};
  }

  ~Logger() {}
};
}

#endif /*__LOGGER_H__*/
