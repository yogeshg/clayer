#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <memory>
#include <type_traits>
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

public:
  // TODO: refrain from passing in 3 things, rather just 1 thing that has all
  // the info we need
  explicit Format(std::ostream &s, const char *Fn, const char *File,
                  int Line)
      : stream(s), fn(Fn), file(File), line(Line) {
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
  Logger(){};

public:
  static Logger &getInstance() {
    static Logger instance;
    return instance;
  }

  template <unsigned int N,
            typename std::enable_if<N >= THRESHOLD>::type * = nullptr>
  Format log(const char *fn, const char *file, int line) {
    return Format(std::cerr, fn, file, line);
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
