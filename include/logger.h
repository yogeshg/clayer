#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <sstream>
#include <memory>
#include <mutex>
#include <type_traits>

namespace logger {
  
static void stream_hex(std::ostream &ss, intptr_t v) {
  auto ff = ss.flags();
  ss << std::hex << std::showbase << v;
  ss.flags(ff);
}

struct ContextInfo {
  const char *file, *fn;
  int line;
};

using Prop = void (*)(std::ostream &, const ContextInfo &);
void prop_func(std::ostream &o, const ContextInfo &i) { o << i.fn; }
void prop_file(std::ostream &o, const ContextInfo &i) { o << i.file; }
void prop_line(std::ostream &o, const ContextInfo &i) { o << i.line; }

// unfortunately can't do template<auto> yet; approved though
// www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0127r1.html
template <const char *fmt, Prop... props>
class Record {
  std::ostream &stream;
  std::lock_guard<std::mutex> lock;
  ContextInfo info;
  intptr_t hash;

  const char *format = fmt;

  // adapted from cppreference parameter_pack page
  void print_prefix() { stream << format; }

  template <typename T, typename... Targs>
  void print_prefix(T head, Targs... tail) {
    while (*format != '\0') {
      if (*(format++) == '%') {
        head(stream, info);
        print_prefix(tail...);
        return;
      }
      stream << *(format - 1);
    }
  }

public:
  Record(std::ostream &s, ContextInfo input_info, std::mutex &Logging_lock)
      : stream(s), info(input_info), lock(Logging_lock), hash(0) {
    print_prefix(props...);
  }
  ~Record() { stream << " "; stream_hex(stream, hash); stream << std::endl; }

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

template <int threshold, const char *fmt, Prop... props>
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
