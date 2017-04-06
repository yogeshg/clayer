#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace logger {

// Lower level destination stream access, one per dest.
template <const str_const D> class FileStream {
  std::ofstream o;
  FileStream() { o(D); }

public:
  static FileStream &get() {
    static FileStream<D> instance;
    return instance;
  }
  void log(std::string s) { o << s; }
};

enum Severity { INFO, DEBUG, WARN, ERROR, CRIT };
// Higher level logger interface parameterized by severity, destination, and later format.
template <Severity S, str_const D> class Logger {
public:
  Logger<S, D> &operator<<(std::string s) { FileStream<D>::get().log(s); return *this; }
};

}

#endif /*__LOGGER_H__*/
