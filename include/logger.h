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

struct ContextInfo {
  const char *file, *fn;
  int line;
};

using Prop = void (*)(std::ostream &, const ContextInfo &);
void prop_msg(std::ostream &o, const ContextInfo &i) { }
void prop_func(std::ostream &o, const ContextInfo &i) { o << i.fn; }
void prop_file(std::ostream &o, const ContextInfo &i) { o << i.file; }
void prop_line(std::ostream &o, const ContextInfo &i) { o << i.line; }

// unfortunately can't do template<auto> yet; approved though
// www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0127r1.html
template <const char *fmt, Prop... props>
class Record {
  std::ostream &stream;
  ContextInfo info;
  std::lock_guard<std::mutex> lock;
  bool done = false;
  intptr_t hash;

  const char *format = fmt;

  // adapted from cppreference parameter_pack page
  void print_prefix() {
    stream << format;
    done = true; // reach end of arg. list. nothing to print after
  }

  template <typename T, typename... Targs>
  void print_prefix(T head, Targs... tail) {
    while (*format != '\0') {
      if (*(format++) == '%') {
        if (head == &prop_msg)
          return;
        head(stream, info);
        print_prefix(tail...);
        return;
      }
      stream << *(format - 1);
    }
    done = true; // reached end of format string; nothing to print after
  }

  void print_suffix() { if (!done) print_prefix(); }

  // resume printing after the prop_msg argument
  template <typename T, typename... Targs>
  void print_suffix(T head, Targs... tail) {
    if (done) return;
    if (head != &prop_msg)
      print_suffix(tail...);
    else
      print_prefix(tail...);
  }

public:
  Record(std::ostream &s, ContextInfo input_info, std::mutex &Logging_lock)
      : stream(s), info(input_info), lock(Logging_lock), hash(0) {
    print_prefix(props...);
  }
  //  << std::hex
  ~Record() { print_suffix(props...); stream << std::endl; }

  template <typename S> Record &operator<<(const S &s) {
    hash ^= intptr_t(&s);
    if (!done) stream << s;
    return *this;
  }
};

class NoRecord {
public:
  template <typename T> NoRecord &operator<<(const T &s) { return *this; }
};

// Would like to have template<auto &stream> here too
// www.open-std.org/jtc2/sc22/wg21/docs/papers/2016/p0127r1.html
template <int threshold, typename T, T &stream, const char *fmt, Prop... props>
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
    return {stream, info, logging_lock};
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
