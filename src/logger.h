#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <memory>

namespace logger {

enum Severity { INFO, DEBUG, WARN, ERROR, CRIT };
template <Severity S, char *D> class Logger {
public:
  Logger &operator<<(string s) { LogStream<D>::get().log(s); }
};

template <char *D> class FileStream {
  ofstream o;
  FileStream() { o(D); }

public:
  static LogStream &get() {
    static FileStream<D> instance;
    return instance;
  }
  void log(string s) { o << s; }
};
}

#endif /*__LOGGER_H__*/
