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

template <typename Stream> using Prop = void (*)(Stream &, const Line &);
using Filter = bool (*)(Line &);

template <typename Stream>
void prop_msg(Stream &o, const Line &l) { o << l.message.str(); }

template <typename Stream>
void prop_hash(Stream &o, const Line &l) {
  auto f = o.flags();
  o << std::hex << std::showbase << l.hash;
  o.flags(f);
}

template <typename Stream>
void prop_func(Stream &o, const Line &l) { o << l.info.fn; }

template <typename Stream>
void prop_file(Stream &o, const Line &l) { o << l.info.file; }

template <typename Stream>
void prop_line(Stream &o, const Line &l) { o << l.info.line; }

namespace hash {
  template <bool Val> class Flag {
    Flag() {}
  public:
    const static Flag<Val> inst;
  };

  template <bool Val> const Flag<Val> Flag<Val>::inst;
  const Flag<true> &on = Flag<true>::inst;
  const Flag<false> &off = Flag<false>::inst;
}

// unfortunately can't do template<auto> yet; approved though
// www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0127r1.html
template <const char *fmt, typename Stream, Prop<Stream>... props>
class Record {
  Stream &stream;
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
  Record(Stream &s, ContextInfo input_info, std::mutex &mutex, Filter filter)
      : stream(s), line(input_info), hash_enabled(true), filter(filter),
        logging_mutex(mutex) {}
  ~Record() {
    std::lock_guard<std::mutex> lock(logging_mutex);
    if ((*filter)(line)) {
      print_fmt(props...);
      stream << std::endl;
    }
  }

  template <bool Val> Record &operator<<(const hash::Flag<Val> &s) {
    hash_enabled = Val;
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
template <int threshold, typename Stream, const char *fmt,
          Prop<Stream>... props>
class Logger {
private:
  std::mutex logging_lock;
  Filter filter;
  Stream &stream;

public:
  Logger(Stream &s) : stream(s) { set_filter(); };
  static Logger &getInstance() {
    static Logger instance(std::clog);
    return instance;
  }

  void set_filter(Filter f = [](Line &l) { return true; }) {
    filter = f;
  }

  template <unsigned int N,
            typename std::enable_if<N >= threshold>::type * = nullptr>
  Record<fmt, Stream, props...> log(ContextInfo info) {
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
