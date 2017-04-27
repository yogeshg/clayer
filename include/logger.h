#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <sstream>
#include <memory>
#include <mutex>
#include <type_traits>

namespace logger {
  
struct ContextInfo {
  const char *file, *fn;
  int line, level;
};

struct Line {
  ContextInfo info;
  std::ostringstream message;
  intptr_t hash;
  Line(const ContextInfo &i) : info(i), message(), hash(0) {}
};

using Prop = void (*)(std::ostream &, const Line &);
using Filter = bool (*)(Line &);

void prop_msg(std::ostream &o, const Line &l) { o << l.message.str(); }
void prop_hash(std::ostream &o, const Line &l) { o << l.hash; }
void prop_func(std::ostream &o, const Line &l) { o << l.info.fn; }
void prop_file(std::ostream &o, const Line &l) { o << l.info.file; }
void prop_line(std::ostream &o, const Line &l) { o << l.info.line; }

template <bool Flag> class HashFlag {
  HashFlag() {}
public:
  const static HashFlag<Flag> inst;
};

template <bool Flag> const HashFlag<Flag> HashFlag<Flag>::inst;
const HashFlag<true> &EnableHash = HashFlag<true>::inst;
const HashFlag<false> &DisableHash = HashFlag<false>::inst;

// unfortunately can't do template<auto> yet; approved though
// www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0127r1.html
template <const char *fmt, Prop... props>
class Record {
  std::ostream &stream;
  Line line;
  bool hash_enabled;
  Filter filter;
  std::mutex &logging_mutex;

  const char *format = fmt;

  // adapted from cppreference parameter_pack page
  void print_fmt() {
    stream << format;
  }

  template <typename T, typename... Targs>
  void print_fmt(T head, Targs... tail) {
    while (*format != '\0') {
      if (*(format++) == '%') {
        head(stream, line);
        print_fmt(tail...);
        return;
      }
      stream << *(format - 1);
    }
  }

public:
  Record(std::ostream &s, ContextInfo input_info, std::mutex &mutex, Filter filter)
      : stream(s), line(input_info), hash_enabled(true), filter(filter),
        logging_mutex(mutex) {}
  ~Record() {
    std::lock_guard<std::mutex> lock(logging_mutex);
    if ((*filter)(line)) {
      print_fmt(props...);
      stream << std::endl;
    }
  }

  template <bool Flag> Record &operator<<(const HashFlag<Flag> &s) {
    hash_enabled = Flag;
    return *this;
  }

  template <typename S> Record &operator<<(const S &s) {
    if (hash_enabled)
      line.hash ^= intptr_t(&s);
    line.message << s;
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
  Filter filter;
  Logger(){};

public:
  static Logger &getInstance() {
    static Logger instance;
    return instance;
  }

  void set_filter(Filter f = [](Line &l) { return true; }) {
    filter = f;
  }

  template <unsigned int N,
            typename std::enable_if<N >= threshold>::type * = nullptr>
  Record<fmt, props...> log(ContextInfo info) {
    return {stream, info, logging_lock, filter};
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
