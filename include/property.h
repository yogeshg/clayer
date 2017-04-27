#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <tuple>

namespace clayer {
  std::regex format_regex = std::regex("\\[(.*) (.*)\\] ([A-Z]*) \\[Thread (0x[0-9a-f]*):(.*)\\((.*):(\\d*)\\)\\]: \\[(.*)\\]");

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
  LINE, // - Line number (CodeContext)
  LEVEL,
  // NAME
  THREAD,
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


struct RunContext {
  std::string level;
  std::string thread;
  std::string date;
  std::string time;

  RunContext() = default;
  RunContext(const std::string &a, const std::string &b,
             const std::string &c, const std::string &d)
  : level(a), thread(b), date(c), time(d) {}
};

static const std::string RunContextStart("");
static const std::string RunContextSep(" ");
static const std::string RunContextEnd("");
std::ostream &operator<<(std::ostream &s, const RunContext &c) {
  return s << RunContextStart << c.level << RunContextSep << c.thread
           << RunContextSep << c.date << RunContextSep << c.time << RunContextEnd;
}

class LogRecord {
public:
  using State = std::pair<CodeContext, RunContext>;
  LogRecord() = default;

  friend std::ostream &operator<<(std::ostream &s, const LogRecord &c);
  CodeContext code;
  RunContext run;
  std::string message;

  State get_state() { return std::make_pair(code, run); }

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

template <> void read_prop<LEVEL>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.run.level;
}
template <> void read_prop<THREAD>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.run.thread;
}
template <> void read_prop<DATE>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.run.date;
}
template <> void read_prop<TIME>(LogRecord &p, const std::string &s) {
  std::istringstream(s) >> p.run.time;
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
  return s << LogRecordStart << c.code << LogRecordSep << c.run <<
           LogRecordSep << c.message << LogRecordEnd;
}

static std::string StateStart("(");
static std::string StateSep(" ; ");
static std::string StateEnd(")");
std::ostream  &operator<<(std::ostream &s,
                          const std::pair<CodeContext, RunContext> &p){
  return s << StateStart << p.first << StateSep << p.second
           << StateEnd;
}


}

#endif /*__PROPERTY_H__*/
