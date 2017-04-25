#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <memory>
#include <mutex>
#include <type_traits>

// * Log record : each line of the log
// * log format : tuple like <Date, Name, Message>
// * log property : Date, Name, Message
#define LOG(severity)                                                          \
  logger::Logger::getInstance().log<logger::severity>(                         \
      {__FILE__, __func__, __LINE__})

std::string hex(intptr_t v) {
  std::stringstream ss;
  ss << std::hex << "0x" << v;
  return ss.str();
}

namespace logger {

// TODO: how to specify THRESHOLD in terms of these? make two header files? NO!
// TODO: JEB suggested we should put these in the log config
enum Severity {
  NOTSET = 0,
  DEBUG = 10,
  INFO = 20,
  WARNING = 30,
  ERROR = 40,
  CRITICAL = 50
};

// auto date = []() { return std::to_string(GetDate()); }
// auto line = [](const Record &f) { return std::to_string(f.line); }

struct ContextInfo {
  const char *file, *fn;
  int line;
};

// Record("(%:%:%)", date, line, ip);
// Record(const char *, f -> string...);
template <const char *Fmt> class Record {
  std::ostream &stream;
  std::lock_guard<std::mutex> lock;
  ContextInfo info;
  intptr_t hash;

  const char *format = Fmt;

  // adapted from cppreference parameter_pack page
  void print_prefix() { stream << format; }

  template <typename T, typename... Targs>
  void print_prefix(T head, Targs... tail) {
    while (*format != '\0') {
      if (*(format++) == '%') {
        stream << head;
        print_prefix(tail...);
        return;
      }
      stream << *(format - 1);
    }
  }

public:
  Record(std::ostream &s, ContextInfo input_info, std::mutex &Logging_lock)
      : stream(s), info(input_info), lock(Logging_lock), hash(0) {
    print_prefix(info.file, info.fn, info.line);
  }
  //  << std::hex
  ~Record() { stream << " " << hex(hash) << std::endl; }

  template <typename S> Record &operator<<(const S &s) {
    const intptr_t address = reinterpret_cast<intptr_t>(&s);
    hash ^= address;
    stream << s;
    return *this;
  }
};

class NoRecord {
public:
  template <typename T> NoRecord &operator<<(const T &s) { return *this; }
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
  Record<format> log(ContextInfo info) {
    return {std::clog, info, logging_lock};
  }

  template <unsigned int N,
            typename std::enable_if<N<THRESHOLD>::type * = nullptr> NoRecord
                log(ContextInfo info) {
    return {};
  }

  ~Logger() {}
};
}

#endif /*__LOGGER_H__*/
