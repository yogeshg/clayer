/**
 * Classes and functions useful for Log Properties
 */
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

/**
 * @brief [items that are contained in a log record]
 */
enum log_properties {
  FILE,           /// File name (CodeContext)
  FUNC,           /// Function name (CodeContext)
  LEVEL,          /// Severity level name (CodeContext)
  LINE,           /// Line number (CodeContext)

// NAME
  DATE,           /// Date on which a Log Record is created (RunContext)
  TIME,           /// Time at which a Log Record is created (RunContext)
  THREAD,         /// Thread in which a LogRecord is created (RunContext)

  HASH,
  MESG            /// Added by the user
};

/**
 * @brief Log properties that can be inferred from code.
 * @details These can be used to answer 'where' in the code base is this log
 * generated from.
 */
struct CodeContext {
  std::string file;
  std::string func;
  std::string level;
  int line;
  std::string hash;

  /**
   * @brief default constructs all the members
   */
  CodeContext() = default;

  /**
   * @brief Constructs the members from explicitly given values
   * 
   * @param a file name
   * @param b function name
   * @param c severity level
   * @param d line number
   * @param e hash value of the log, typically xor of memory locations of records
   */
  CodeContext(const std::string &a, const std::string &b, const std::string &c,
            const int &d, const std::string &e)
      : file(a), func(b), level(c), line(d), hash(e) {}

  /**
   * @brief Returns a tuple of all the members
   * @details This returns a serialisable representation that can be used to
   * print or order this class
   * 
   * @returns a tuple with all the constituents
   */
  decltype(auto) tie() const { return std::tie(file, func, level, line, hash); }

  /**
   * @brief Compares two CodeContext using their tuple representation
   * @details first compares file name, if same then compare function name,
   * if same then level and so on.
   * 
   * @param rhs CodeContext to be compared with.
   * @return if the lhs is smaller than the rhs.
   */
  bool operator<(const CodeContext &rhs) const { return tie() < rhs.tie(); }
};

/**
 * @brief prints CodeContext c to an out-stream ss
 * 
 * @param ss reference to a stream to which c should be printed
 * @param c CodeContext to be printed to the stream
 * @return reference to the stream to which c was printed
 */
std::ostream &operator<<(std::ostream &ss, const CodeContext &c) {
  return util::to_string(ss, c.tie());
}

/**
 * @brief Log properties that are inferred when the code is run .
 * @details These can be used to answer 'when' in the runtime was the log
 * generated.
 */
struct RunContext {
  std::string date;
  std::string thread;
  std::string time;

  /**
   * @brief default constructs all the members
   */
  RunContext() = default;

  /**
   * @brief Constructs the members from explicitly given values
   * 
   * @param a date
   * @param b time
   * @param c thread identifier
   */  
  RunContext(const std::string &a, const std::string &b, const std::string &c)
      : date(a), thread(b), time(c) {}


  /**
   * @brief Returns a tuple of all the members
   * @details This returns a serialisable representation that can be used to
   * print or order this class
   * 
   * @returns a tuple with all the constituents
   */
  decltype(auto) tie() const { return std::tie(date, thread, time); }

  /**
   * @brief Compares two RunContext using their tuple representation
   * @details first compares date, if same then compare function thread,
   * if same then time.
   * 
   * @param rhs RunContext to be compared with.
   * @return if the lhs is smaller than the rhs.
   */
  bool operator<(const RunContext &rhs) const { return tie() < rhs.tie(); }
};


/**
 * @brief prints RunContext c to an out-stream ss
 * 
 * @param ss reference to a stream to which c should be printed
 * @param c RunContext to be printed to the stream
 * @return reference to the stream to which c was printed
 */
std::ostream &operator<<(std::ostream &ss, const RunContext &r) {
  return util::to_string(ss, r.tie());
}

/**
 * @brief Each line in a log file can be considered a Log Record
 * @details Object of this class contains all the information that can be
 * retrieved from a log
 */
class LogRecord {
public:

  using State = std::pair<CodeContext, RunContext>;

  /**
   * @brief default constructs all the members
   */
  LogRecord() = default;

  /**
   * @brief prints LogRecord c to an out-stream ss
   * 
   * @param ss reference to a stream to which c should be printed
   * @param c LogRecord to be printed to the stream
   * @return reference to the stream to which c was printed
   */
  friend std::ostream &operator<<(std::ostream &s, const LogRecord &c);

  CodeContext code;               /// Code Context of the log record
  RunContext run;                 /// Run Context of the log record
  std::string message;            /// message of the log record
  std::vector<float> numbers;     /// numbers in the message of the log record

  /**
   * @brief State of a log record is something that which you can use to 'Group By'
   * @return the state of a log record
   * @see typename State
   */
  State get_state() { return std::make_pair(code, run); }

  // decltype(auto) tie() const { return std::tie(code, message); }



  /**
   * @details This returns a serialisable representation
   * @returns a tuple of code context and run context
   */
  decltype(auto) tie() const { return std::tie(code, run); }

private:
  // TODO: make code private and try to make read_prop friendly
  // NOTE: lmgtfy.com?q=one cannot declare a namespace within a class
  // RunContext prog;
};

/**
 * @brief reads the property (on which the function is templates) into
 * a log record p from a string s
 * 
 * @param p log record to read into
 * @param s string to read from
 */
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

/**
 * @brief reads all the properties into a log record p from a string s
 * @details reads only the properties on which the function is templates
 * and in the order that the template appears in
 * 
 * @param p log record to read into
 * @param s string to read from
 */
template <log_properties... I>
void parse_props(LogRecord &p, std::string &line, 
    std::regex format_regex = std::regex("(.*)\\((.*):(.*)\\):(.*)")) {
  std::smatch m;
  std::regex_match(line, m, format_regex);
  // std::cout << m.size() << " " << sizeof...(I) << "\n";
  read_props<std::smatch::iterator, I...>(p, m.begin() + 1);
}



// * LogRecord can be generated by a CodeContext - "Where", or a RunContext - "When"

template<log_properties prop> decltype(auto) get_prop(const LogRecord& rec);

template<> decltype(auto) get_prop<FILE>(const LogRecord& rec) {return rec.code.file;}
template<> decltype(auto) get_prop<FUNC>(const LogRecord& rec) {return rec.code.func;}
template<> decltype(auto) get_prop<LEVEL>(const LogRecord& rec) {return rec.code.level;}
template<> decltype(auto) get_prop<LINE>(const LogRecord& rec) {return rec.code.line;}
template<> decltype(auto) get_prop<DATE>(const LogRecord& rec) {return rec.run.date;}
template<> decltype(auto) get_prop<TIME>(const LogRecord& rec) {return rec.run.time;}
template<> decltype(auto) get_prop<THREAD>(const LogRecord& rec) {return rec.run.thread;}

template<typename T = void>
std::ostream&  get_props(std::ostream&, const LogRecord&);

template<>
std::ostream&  get_props<>(std::ostream& os, const LogRecord& rec){
    return os;
}

/**
 * @brief gets all the properties from a log record p and streams to an outstream
 * @details reads only the properties on which the function is templates
 * and in the order that the template appears in
 * 
 * @param os outstream to stream to
 * @param rec log record to get from
 * @returns outstream that has been written to
 */
template<log_properties head, log_properties... I>
std::ostream&  get_props(std::ostream& os, const LogRecord& rec) {
    os << get_prop<head>(rec)<<" ";
    get_props<I...>(os, rec);
    return os;
}

// std::ostream& get_props_test(std::ostream& os, const LogRecord& rec) {
//   return get_props<FILE, FUNC>(os, rec);
// }


/**
 * @brief streams a log record to an outstream
 * 
 * @param os outstream to stream to
 * @param lr log record to stream
 */
std::ostream &operator<<(std::ostream &os, const LogRecord &lr) {
  return util::to_string(os, lr.tie()) << lr.message;
}

/**
 * @brief streams the state of a log record to an outstream
 * 
 * @param os outstream to stream to
 * @param p state of a log record
 */
std::ostream &operator<<(std::ostream &os,
                          const std::pair<CodeContext, RunContext> &p){

  return util::to_string(os, std::tuple<CodeContext, RunContext>(p));
}
}

#endif /*__PROPERTY_H__*/
