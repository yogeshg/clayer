#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <tuple>

namespace clayer {

std::regex format_regex = std::regex("(.*)\\((.*):(.*)\\):(.*)");

// Concepts used for property reading and writing
template <typename T> concept bool Streamable = requires(T o, std::ostream &s) {
  { s << o } -> std::ostream &;
};

template <typename T> concept bool StringRef = requires(T o) {
  { *o } -> std::string;
};

// template<typename T=void>
// void print<>(){
//     std::cout << "\n";
// }

// template<Streamable T, typename... Targs>
// void print(T o, Targs... tail) {
//     std::cout << o <<" ";
//     print(tail...);
// }

// ## Log LogRecord - Any item that can be printed in a Log Record
enum log_properties {
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

// ### Code Context - Log Properties that can be inferred from Code
struct CodeContext {
  std::string file;
  std::string func;
  int line;

  CodeContext() = default;
  CodeContext(const std::string &a, const std::string &b, const int &c)
      : file(a), func(b), line(c) {}

  decltype(auto) tie() const { return std::tie(file, func, line); }
  bool operator<(const CodeContext &rhs) const { return tie() < rhs.tie(); }
};
static const std::string CodeContextStart("");
static const std::string CodeContextSep(" ");
static const std::string CodeContextEnd("");
std::ostream &operator<<(std::ostream &s, const CodeContext &c) {
  return s << CodeContextStart << c.file << CodeContextSep << c.func
           << CodeContextSep << c.line << CodeContextEnd;
}

// TODO: RunContext

class LogRecord {
public:
  using State = CodeContext;
  LogRecord() : code(){};

  friend std::ostream &operator<<(std::ostream &s, const LogRecord &c);

  CodeContext code;
  std::string message;

  State get_state() { return code; }

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

template <> void read_prop<LINE>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.code.line;
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
void parse_props(LogRecord &p, std::string &line) {
  std::smatch m;
  std::regex_match(line, m, format_regex);
  read_props<std::smatch::iterator, I...>(p, m.begin() + 1);
}

static std::string LogRecordStart("(");
static std::string LogRecordSep(" ");
static std::string LogRecordEnd(")");
std::ostream &operator<<(std::ostream &s, const LogRecord &c) {
  return s << LogRecordStart << c.code << LogRecordSep << c.message
           << LogRecordEnd;
}
}

#endif /*__PROPERTY_H__*/
