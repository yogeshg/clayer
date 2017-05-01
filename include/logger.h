/**
 * Class and function definitions to facilitate logging and formatting.
 */
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <sstream>
#include <memory>
#include <mutex>
#include <type_traits>

namespace logger {

/**
 * @brief A Stream is something that implements << for strings at the very
 * least. More generally it can implement << for any type, but for we require <<
 * for strings given the context in a logging library.
 */
template <typename T> concept bool Stream = requires(T o, std::string s) {
  { o << s } -> T &;
};

/**
 * @brief Reproduced in property.h, something is Streamable if an ostream can
 * stream it. We don't refer to our Stream concept above because practically in
 * our library every Stream forwards to some underlying ostream.
 */
template <typename T> concept bool Streamable = requires(T o, std::ostream &s) {
  { s << o } -> std::ostream &;
};

/**
 * @brief A plain-old-data container for the fixed code context of a log record.
 * Necessary to propagate context information from the predefined macros.
 * Auxiliary, short-lived class with life-time the duration of the log command.
 */
struct ContextInfo {
  // A pointer to the filename containing the log command.
  const char *file;

  // A pointer to the function name containing the log command.
  const char *fn;

  // Values representing the line of the log command and the severity level at
  // which it was invoked.
  int line, level;
};

/**
 * @brief A structure representing all the information about a particular
 * logging line. Includes contextual and message information.
 */
struct Line {
  // Information about the context of the recorded line: file, function,
  // severity, line number.
  ContextInfo info;

  // A local buffer for the message corresponding to this log command.
  std::ostringstream message;

  // A hash that serves as an identifier for the message. It depends on the
  // identifiers (for now, addresses) of the components of the message (the
  // variables and strings output)
  intptr_t hash;

  /**
   * @brief Constructs a Line from a ContextInfo with initial empty message and
   * hash values. A ContextInfo is necessary to provide the initial state of the
   * Line.
   *
   * @param i The filename, function name, line number, and severity level
   * information.
   */
  Line(const ContextInfo &i) : info(i), message(), hash(0) {}
};

// Type aliases for functions used to manipulate logging output.

/**
 * @brief Prop represents contextual properties printable in logs as part of the
 * formatting.
 *
 * @remark As a side note, most of the Props defined in this file extract
 * information from the Line directly, while custom Props would often supply
 * additional contextual information.
 *
 * @param Stream the logging stream to output to.
 * @param Line the line containing the information to output to the stream.
 */
template <typename Stream> using Prop = void (*)(Stream &, const Line &);

/**
 * @brief Filter represents post-processing on Lines that could entail content
 * manipulation (modifying the argument) or deletion (returning false).
 *
 * @param Line the line to transform and possibly reject.
 * @return a boolean specifying whether or not to finally output the line to the
 * destination stream.
 */
using Filter = bool (*)(Line &);

/**
 * @brief A Prop that prints the message component to a log stream.
 */
template <typename Stream> void prop_msg(Stream &o, const Line &l) {
  o << l.message.str();
}

/**
 * @brief A Prop that prints the hash identifier in hexadecimal of a log to a
 * log stream.
 */
template <typename Stream> void prop_hash(Stream &o, const Line &l) {
  auto f = o.flags();
  o << std::hex << std::showbase << l.hash;
  o.flags(f);
}

/**
 * @brief A Prop that prints the function name to a log stream.
 */
template <typename Stream> void prop_func(Stream &o, const Line &l) {
  o << l.info.fn;
}

/**
 * @brief A Prop that prints the filename to a log stream.
 */
template <typename Stream> void prop_file(Stream &o, const Line &l) {
  o << l.info.file;
}

/**
 * @brief A Prop that prints the line number to a log stream.
 */
template <typename Stream> void prop_line(Stream &o, const Line &l) {
  o << l.info.line;
}

/**
 * @brief Functionality related to specifying if a component of a log message
 * should influence the hash of the message or not.
 */
namespace hash {
/**
 * @brief Template class for template overloading hash settings in logger
 * stream operators.
 *
 * @remark Private constructor so that only one instance of each class exists.
 */
template <bool Val> class Flag {
  Flag() {}

public:
  const static Flag<Val> inst;
};

/**
 * @brief Static singleton instances for the two boolean hash configuration
 * states.
 *
 * @usage LOG(INFO) << hash::on << "Amount: " << hash::off << 20 << std::endl;
 */
template <bool Val> const Flag<Val> Flag<Val>::inst;
const Flag<true> &on = Flag<true>::inst;
const Flag<false> &off = Flag<false>::inst;
}

/**
 * @brief A catch-all class for storing a log line record and printing it
 * according to a format.
 *
 * @detailed The cornerstone of the logging library. This class takes contextual
 * information about a log call, along with auxiliary structures like a mutex
 * and a filter, and provides a way to stream message data to the log. When its
 * time comes (at the end of the logging command, if used properly), it'll flush
 * its local buffer into the stream passed to it.
 *
 * @remark This hints at why Prop must be a type alias instead of a concept.
 * Record takes in instances of Prop as template parameters, and there's no way
 * to take in instances of variable types as template parameters (no template
 * for template parameters) as of yet (If Prop were a concept, props would have
 * to be a list of types). A similar problem necessitates puting Stream as an
 * explicit template parameter. This would be resolved by a proposal approved
 * for C++17 to be implemented in GCC 7 [1].
 *
 * [1] www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0127r1.html
 */
template <const char *fmt, typename Stream, Prop<Stream>... props>
class Record {
  /**
   * @brief A reference to the stream to dump to from the local buffer once the
   * record is completed.
   */
  Stream &stream;

  /**
   * @brief The Line containing all the information about the log line: context,
   * message, and hash.
   */
  Line line;

  /**
   * @brief A flag indicating whether the content currently being streamed to
   * the log record should influence the hash value or not.
   */
  bool hash_enabled;

  /**
   * @brief The Filter to be applied to the Record before printing out.
   *
   * @remark Because it's dynamically configurable the type must be fixed (so
   * it's a fixed type alias instead of a concept template argument, but
   * generally filters won't need the full capture capability of lambdas.
   */
  Filter filter;

  /**
   * @brief A reference to the mutex for the logger for synchronization.
   */
  std::mutex &logging_mutex;

  /**
   * @brief A **pointer** to the format string for traversal when printing.
   */
  const char *format = fmt;

  // adapted from cppreference parameter_pack page
  /**
   * @brief Prints the properties in the format list according to the format
   * string.
   *
   * @remark The real workhorse of this class. Using variadic templates and
   * clever matching, print_fmt prints all of the format string up until a `%`
   * symbol, at which it looks at the first Prop argument and executes it on the
   * local buffer and the Line info. It does a recursive call with the remaining
   * arguments to format the rest of the string. The first definition is
   * analogous to the base case and the second the inductive case.
   *
   * @param head The next Prop argument to print.
   * @param tail The remaining Prop arguments.
   */
  inline void print_fmt() { stream << format; }

  template <typename T, typename... Targs>
  inline void print_fmt(T head, Targs... tail) {
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
  /**
   * @brief Constructs a Record from relevant information.
   *
   * @param s the destination to which to stream the record output.
   * @param input_info the contextual information about the log command.
   * @param mutex the lock to control synchronization to the output stream.
   * @param filter the post-processing filter for the log message.
   */
  Record(Stream &s, ContextInfo input_info, std::mutex &mutex, Filter filter)
      : stream(s), line(input_info), hash_enabled(true), filter(filter),
        logging_mutex(mutex) {}

  /**
   * @brief Locks the stream mutex and dumps the local buffer to the stream,
   * after applying the final filter. In the class's standard usage, Records
   * live only as rvalues without any names, so that they're destroyed at the
   * end of the log statement.
   */
  ~Record() {
    std::lock_guard<std::mutex> lock(logging_mutex);
    if ((*filter)(line)) {
      print_fmt(props...);
      stream << std::endl;
    }
  }

  /**
   * @brief Stream an object to the local buffer. Accepts anything a
   * stringstream would accept. Also modifies the hash identifier if the option
   * is enabled.
   *
   * @param s The object to be streamed.
   * @return The same record, for stringing stream commands.
   */
  template <Streamable S> Record &operator<<(const S &s) {
    if (hash_enabled)
      line.hash ^= intptr_t(&s);
    line.message << s;
    return *this;
  }

  /**
   * @brief Set the flag that determines whether or not subsequent objects to be
   * streamed should influence the hash or not. Similar to std::hex.
   *
   * @param s Either hash::on or hash::off; they're the only instances of types
   * matching this overload.
   *
   * @usage LOG(DEBUG) << hash::on << "Hey";
   * @return The same record, for stringing stream commands.
   */
  template <bool Val> Record &operator<<(const hash::Flag<Val> &s) {
    hash_enabled = Val;
    return *this;
  }
};

/**
 * @brief A record that eats and throws away objects streamed to it. Used for
 * compile-time filtering. Can also be used as a no-op streamer.
 */
class NoRecord {
public:
  template <typename T> NoRecord &operator<<(const T &s) { return *this; }
};

/**
 * @brief The wrapper/aggregator class for logging commands.
 * Template-parameterized by format and logging threshold for compile-time
 * configuration.
 *
 * @remark As with the Record definition, Prop is a fixed type alias as auto
 * template parameters aren't possible in GCC yet.
 *
 * @remark We tried to pinpoint a concept for Stream but failed because the only
 * reasonable description for Stream would be that it implements the stream
 * operator, so it might make sense to require a Stream to be able to << a a
 * string, but even then some streams may only work on particular types, for
 * example a bit or number stream.
 */
template <typename Stream, int threshold, const char *fmt,
          Prop<Stream>... props>
class Logger {
private:
  /**
   * @brief The lock associated with the logger, to prevent concurrent access to
   * the stream. In the future, we hope to use a lock-free queue.
   */
  std::mutex logging_lock;

  /**
   * @brief The underlying stream to which we log.
   */
  Stream &stream;

  /**
   * @brief An optional filter that post-processes log records, possibly
   * throwing them out if they don't satisfy the condition.
   */
  Filter filter;

public:
  /**
   * @brief Constructs a logger from a stream.
   *
   * @param s The stream to which to log.
   */
  Logger(Stream &s) : stream(s), filter([](Line &l) { return true; }){};

  /**
   * @brief Set the filter for the log.
   *
   * @param A boolean function taking in a reference to a Line and outputting a
   * boolean, so it can modify and report whether to finally output or not.
   */
  void set_filter(Filter f) { filter = f; }

  /**
   * @brief Constructs a logging record from contextual information when the
   * logging level exceeds the threshold of the logger.
   *
   * @param N the level at which to log.
   * @param info Contextual information about the log statement: line, file, and
   * function name.
   *
   * @return A Record with the logger's format and specified level that can be
   * streamed to.
   */
  template <unsigned int N,
            typename std::enable_if<N >= threshold>::type * = nullptr>
  Record<fmt, Stream, props...> log(ContextInfo info) {
    return {stream, info, logging_lock, filter};
  }

  /**
   * @brief For logging levels below the threshold of the logger, this overload
   * is called and does nothing.
   *
   * @param N logging level, below the threshold of the logger.
   * @param info Ignored.
   *
   * @return A NoRecord that does nothing when you stream to it.
   */
  template <unsigned int N,
            typename std::enable_if<N<threshold>::type * = nullptr> NoRecord
                log(ContextInfo info) {
    return {};
  }
};
}

#endif /*__LOGGER_H__*/
