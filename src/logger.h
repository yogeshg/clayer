#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <memory>

namespace logger {
template <LogFmt F, Severity S, char *D> class Logger {
public:
  Logger &operator<<(string s) { LogStream<D>::get().log(F::format(s)); }
};

template <char *D> class FileStream {
  static LogStream instance;
  ofstream o;
  LogStream() { o(D); }

public:
  static LogStream &get() {
    return instance;
  }
  void log(string s) { o << s; }
};
}

#endif /*__LOGGER_H__*/
