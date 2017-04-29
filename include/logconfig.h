/**
 * Additional sample definitions, class instantiations, and macro definitions
 * for easier use.
 */
#ifndef __LOGCONFIG_H__
#define __LOGCONFIG_H__

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

#include "logger.h"

namespace logger {
/**
 * @brief Sample severity levels for logging commands and classes.
 */
enum Severity {
  NOTSET = 0,
  DEBUG = 10,
  INFO = 20,
  WARNING = 30,
  ERROR = 40,
  CRITICAL = 50
};

/**
 * @brief A Prop that prints out a string corresponding to the severity level
 * for a given Line.
 */
template <typename Stream>
void prop_level(Stream &o, const Line &l) {
  switch (l.info.level) {
    case 0: o << "NOTSET"; break;
    case 10: o << "DEBUG"; break;
    case 20: o << "INFO"; break;
    case 30: o << "WARNING"; break;
    case 40: o << "ERROR"; break;
    case 50: o << "CRITICAL"; break;
    default: o << l.info.level;
  }
}

// Can avoid the Line parameter in GCC 7 with template <auto> elsewhere
/**
 * @brief A Prop that prints the current time to the log stream.
 */
template <typename Stream>
void prop_time(Stream &o, const Line &l) {
  using std::chrono::system_clock;
  auto now = system_clock::to_time_t(system_clock::now());
  o << std::put_time(std::localtime(&now), "%T");
}

/**
 * @brief A Prop that prints the current date to the log stream.
 */
template <typename Stream>
void prop_date(Stream &o, const Line &l) {
  using std::chrono::system_clock;
  auto now = system_clock::to_time_t(system_clock::now());
  o << std::put_time(std::localtime(&now), "%F");
}

/**
 * @brief A Prop that prints an identifier for the current thread to the log
 * stream.
 */
template <typename Stream>
void prop_thread(Stream &o, const Line &l) {
  auto f = o.flags();
  o << std::hex << std::showbase << std::this_thread::get_id();
  o.flags(f);
}

/**
 * @brief Sample format strings: one with placeholders for several Props, the
 * other for just a single Prop.
 */
constexpr const char full_fmt[] =
    "\033[1;31m[% %]\033[0m %[Thread %:%(%:%)]: [%] [%]";

constexpr const char basic_fmt[] = "%";

/**
 * @brief A sample logger that prints full contextual information along with
 * each message.
 *
 * @usage FullLogger<std::clog, DEBUG> my_logger; CLOG(my_logger, WARN) << "hi";
 */
template <typename Stream, int threshold = INFO>
using FullLogger =
    Logger<Stream, threshold, full_fmt, prop_date, prop_time, prop_level,
           prop_thread, prop_file, prop_func, prop_line, prop_msg, prop_hash>;

/**
 * @brief A sample logger that prints just the log message.
 *
 * @usage BasicLogger<std::clog, DEBUG> logger; CLOG(logger, WARN) << "hi";
 */
template <typename Stream, int threshold = INFO>
using BasicLogger = Logger<Stream, threshold, basic_fmt, prop_msg>;

template <const char *fmt, Prop<std::ostream>... props>
using FmtLogger = Logger<std::ostream, INFO, fmt, props...>;
}

/**
 * @brief A default logger LOG, along with macros to call loggers with
 * pre-populated contexts. Macros are necessary since there's no other way to
 * hook into the filename/function name/line number of the calling site.
 */
logger::FullLogger<std::ostream> LOG(std::clog);
#define LOG(severity) CLOG(LOG, severity)
#define CLOG(instance, severity) CLOGL(instance, logger::severity)
#define CLOGL(instance, severity)                                              \
  instance.log<severity>({__FILE__, __func__, __LINE__, severity})

#endif /* __LOGCONFIG_H__ */
