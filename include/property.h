#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <tuple>
#include "util.h"

namespace clayer {

// Concepts used for property reading and writing
template <typename T> concept bool Streamable = requires(T o, std::ostream &s) {
  { s << o } -> std::ostream &;
};

template <typename T> concept bool StringRef = requires(T o) {
  { *o } -> std::string;
};

// ## Log LogRecord - Any item that can be printed in a Log Record
enum log_properties {
  FILE, // - File name (CodeContext)
  FUNC, // - Function name (CodeContext)
  LEVEL,
  LINE, // - Line number (CodeContext)
  HASH,
  // NAME

  DATE, // - Date a Log Record is created (RunContext)
  TIME, // - Time a Log Record is created (RunContext)
  THREAD,

  MESG  // - Added by the user
  // prop_time, prop_level, prop_thread, prop_file, prop_func, prop_line, prop_msg
};

// ### Code Context - Log Properties that can be inferred from Code
struct CodeContext {
  std::string file;
  std::string func;
  std::string level;
  int line;
  std::string hash;

  CodeContext() = default;
  // CodeContext(const std::string &a, const std::string &b, const std::string &c,
  //           const int &d, const std::string &e)
  //     : file(a), func(b), level(c), line(d), hash(e) {}

  decltype(auto) tie() const { return std::tie(file, func, level, line, hash); }
  bool operator<(const CodeContext &rhs) const { return tie() < rhs.tie(); }
};

std::ostream &operator<<(std::ostream &ss, const CodeContext &c) {
  return util::to_string(ss, c.tie());
}

struct RunContext {
  std::string date;
  std::string time;
  std::string thread;
  RunContext() = default;
  // RunContext(const std::string &a, const std::string &b, const std::string &c)
  //     : date(a), time(b), thread(c) {}

  decltype(auto) tie() const { return std::tie(date, time, thread); }
  bool operator<(const RunContext &rhs) const { return tie() < rhs.tie(); }
};
std::ostream &operator<<(std::ostream &ss, const RunContext &r) {
  return util::to_string(ss, r.tie());
}

class LogRecord {
public:
  using State = CodeContext;
  LogRecord() : code(){};

  friend std::ostream &operator<<(std::ostream &s, const LogRecord &c);

  CodeContext code;
  RunContext run;
  std::string message;

  State get_state() { return code; }
  // decltype(auto) tie() const { return std::tie(code, message); }
  decltype(auto) tie() const { return std::tie(code, run, message); }

private:
  // TODO: make code private and try to make read_prop friendly
  // NOTE: lmgtfy.com?q=one cannot declare a namespace within a class
  // RunContext prog;
};

// ### Methods to read Properties for the analyser

template <log_properties N> void read_prop(LogRecord &p, const std::string &s);

template <> void read_prop<FILE>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.code.file;
}
template <> void read_prop<FUNC>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.code.func;
}

template <> void read_prop<LEVEL>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.code.level;
}

template <> void read_prop<LINE>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.code.line;
}

template <> void read_prop<HASH>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.code.hash;
}

template <> void read_prop<DATE>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.run.date;
}

template <> void read_prop<TIME>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.run.time;
}

template <> void read_prop<THREAD>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.run.thread;
}

template <> void read_prop<MESG>(LogRecord &p, const std::string &s) {
  p.message = s;
}

// TODO: StringRef -> forward iterator on std::string
template <StringRef PS> void read_props(LogRecord &p, PS) {}

template <StringRef PS, log_properties head, log_properties... I>
void read_props(LogRecord &p, PS ps) {
  read_prop<head>(p, *ps);
  ps++;
  read_props<PS, I...>(p, ps);
}

template <log_properties... I>
void parse_props(LogRecord &p, std::string &line, 
    std::regex format_regex = std::regex("(.*)\\((.*):(.*)\\):(.*)")) {
  std::smatch m;
  std::regex_match(line, m, format_regex);
  // std::cout << m.size() << " " << sizeof...(I) << "\n";
  read_props<std::smatch::iterator, I...>(p, m.begin() + 1);
}

std::ostream &operator<<(std::ostream &ss, const LogRecord &lr) {
  return util::to_string(ss, lr.tie());
}
}

#endif /*__PROPERTY_H__*/
