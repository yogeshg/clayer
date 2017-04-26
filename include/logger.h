#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <sstream>
#include <memory>
#include <mutex>
#include <type_traits>

std::string hex(intptr_t v) {
  std::stringstream ss;
  ss << std::hex << "0x" << v;
  return ss.str();
}

namespace logger {

template <typename T> concept bool Streamable = requires(T o, std::ostream &s) {
  { s << o } -> std::ostream &;
};

struct ContextInfo {
  const char *file, *fn;
  int line;
};

enum property {
  FILE, // - File name (CodeContext)
  FUNC, // - Function name (CodeContext)
  LINE, // - Line number (CodeContext)
  // LEVEL
  // NAME
  // THREAD
  // HASH
  DATE, // - Date a Log Record is created (RunContext)
  TIME, // - Time a Log Record is created (RunContext)
  MESG  // - Added by the user
};

template <property prop> void stream_prop(std::ostream &o, const ContextInfo &i) {}
template <> void stream_prop<FILE>(std::ostream &o, const ContextInfo &i) {
  o << i.file;
}
template <> void stream_prop<FUNC>(std::ostream &o, const ContextInfo &i) {
  o << i.fn;
}
template <> void stream_prop<LINE>(std::ostream &o, const ContextInfo &i) {
  o << i.line;
}

// unfortunately can't do template<auto> yet; approved though
// www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0127r1.html
template <const char *fmt, property... props>
class Record {
  std::ostream &stream;
  std::lock_guard<std::mutex> lock;
  ContextInfo info;
  intptr_t hash;

  const char *format = fmt;

  // adapted from cppreference parameter_pack page
  void print_prefix() { stream << format; }

  template <property Head, property... Tail> void print_prefix() {
    while (*format != '\0') {
      if (*(format++) == '%') {
        stream_prop<Head>(stream, info);
        print_prefix<Tail...>();
        return;
      }
      stream << *(format - 1);
    }
  }

public:
  Record(std::ostream &s, ContextInfo input_info, std::mutex &Logging_lock)
      : stream(s), info(input_info), lock(Logging_lock), hash(0) {
    print_prefix<props...>();
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

template <int threshold, const char *fmt, property... props>
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
            typename std::enable_if<N >= threshold>::type * = nullptr>
  Record<fmt, props...> log(ContextInfo info) {
    return {std::clog, info, logging_lock};
  }

  template <unsigned int N,
            typename std::enable_if<N<threshold>::type * = nullptr> NoRecord
                log(ContextInfo info) {
    return {};
  }

  ~Logger() {}
};
}

#endif /*__LOGGER_H__*/
