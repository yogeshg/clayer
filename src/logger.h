#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <memory>
#include <type_traits>
#include <mutex>

// * Log record : each line of the log
// * log format : tuple like <Date, Name, Message>
// * log entity : Date, Name, Message
#define LOG(severity)                                                          \
  logger::Logger::getInstance().log<logger::severity>(                         \
      {__func__, __FILE__, __LINE__})

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

enum Info { DATE, SEVERITY, LINE };

// auto date = []() { return std::to_string(GetDate()); }
// auto line = [](const Format &f) { return std::to_string(f.line); }

struct InputInfo {
  const char *file, *fn;
  int line;
};

// Format("(%:%:%)", date, line, ip);
// Format(const char *, f -> string...);
template <const char *Fmt>
class Format {
  std::ostream &stream;
  std::lock_guard<std::mutex> lock;
  InputInfo info;
  const char *format = Fmt;

  // adapted from cppreference parameter_pack page
  void print_prefix() {
    stream << format;
  }

  template<typename T, typename... Targs>
  void print_prefix(T head, Targs... tail) {
    while (*format != '\0') {
      if (*(format++) == '%') {
        stream << head;
        print_prefix(tail...);
        return;
      }
      stream << *(format-1);
    }
  }

public:
  Format(std::ostream &s, InputInfo input_info,
                  std::mutex &Logging_lock)
      : stream(s), info(input_info), lock(Logging_lock) {
      print_prefix(info.file, info.fn, info.line);
  }
  ~Format() { stream << std::endl; }

  template <typename S> Format &operator<<(const S &s) {
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
  Format log(InputInfo info) {
    return {std::clog, info, logging_lock};
  }

  template <unsigned int N,
            typename std::enable_if<N<THRESHOLD>::type * = nullptr> NoFormat
                log(InputInfo info) {
    return {};
  }

  ~Logger() {}
};
}

#endif /*__LOGGER_H__*/
