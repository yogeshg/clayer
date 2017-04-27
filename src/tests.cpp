#include <iostream>
#include <thread>

#include "logconfig.h"
#include "logger.h"

namespace test {
using namespace logger;

enum Status { PASS, FAIL };

template <typename Stream>
void prop_testno(Stream &o, const Line &l) {
  static int test_no = 0;
  static int success_no = 0;
  if (l.info.level == PASS) ++success_no;
  o << "\033[1m" << success_no << "/" << ++test_no << "\033[0m";
}

template <typename Stream>
void prop_stat(Stream &o, const Line &l) {
  switch (l.info.level) {
    case PASS: o << "\033[1;32mPASS\033[0m"; break;
    case FAIL: o << "\033[1;31mFAIL\033[0m"; break;
    default: o << "\033[1;37m????\033[0m"; break;
  }
}

constexpr const char min_fmt[] = R"([%] % - "%")";
Logger<NOTSET, std::ostream, min_fmt, prop_testno, prop_stat, prop_msg>
    TestLogger(std::cout);

template <typename F>
class Test {
  std::string desc_;
  F test_;
  public:
  Test(const std::string &desc, const F &test) : desc_(desc), test_(test) {};
  void operator()() {
    if (test_())
      CLOGL(TestLogger, PASS) << desc_;
    else
      CLOGL(TestLogger, FAIL) << desc_;
  }
};

template <typename F>
Test<F> make(const std::string &desc, const F &test) {
  return Test<F>(desc, test);
}
}

namespace logger {
constexpr const char my_format[] = "[%] %[%:%(%:%)]: [%]";
using MyLogger = Logger<DEBUG, std::ostringstream, my_format, prop_time, prop_level,
                        prop_thread, prop_file, prop_func, prop_line, prop_msg>;
}

void test_basic() {
  test::make("Prints the message normally", []() {
    std::ostringstream x;
    logger::BasicLogger<std::ostringstream> Logger(x);

    CLOG(Logger, ERROR) << "broke down";
    auto p = x.str().find("broke down");

    return (p != std::string::npos);
  })();

  test::make("Prints multiple messages in order", []() {
    std::ostringstream x;
    logger::BasicLogger<std::ostringstream> Logger(x);

    CLOG(Logger, ERROR) << "broke up";
    CLOG(Logger, WARNING) << "broke down";
    auto p = x.str().find("broke up");

    return (p != std::string::npos &&
            x.str().find("down", p + 1) != std::string::npos);
  })();

  test::make("Prints nothing when threshold too high", []() {
    std::ostringstream x;
    logger::BasicLogger<std::ostringstream> Logger(x);

    CLOG(Logger, DEBUG) << "just telling you";

    return x.str().empty();
  })();

  test::make("Should filter out messages below threshold", []() {
    std::ostringstream x;
    logger::BasicLogger<std::ostringstream> Logger(x);

    CLOG(Logger, WARNING) << "serious";
    CLOG(Logger, NOTSET) << "funny";

    return (x.str().find("serious") != std::string::npos &&
            x.str().find("funny") == std::string::npos);
  })();
}

void test_props() {
  using namespace logger;
  test::make("Can omit messages if not in format", []() {
    std::ostringstream x;
    logger::Logger<DEBUG, std::ostringstream, basic_fmt, prop_level> Logger(x);

    CLOG(Logger, ERROR) << "broke down";

    return (x.str().find("broke down") == std::string::npos);
  })();

  test::make("Correctly prints preset log level strings", []() {
    std::ostringstream x;
    logger::Logger<DEBUG, std::ostringstream, basic_fmt, prop_level> Logger(x);

    CLOG(Logger, ERROR) << "broke down";
    CLOG(Logger, WARNING) << "broke down again";

    return (x.str().find("ERROR") != std::string::npos &&
            x.str().find("WARNING") != std::string::npos);
  })();

  test::make("Correctly prints thread while preserving number format", []() {
    std::ostringstream x;
    logger::Logger<DEBUG, std::ostringstream, my_format, prop_thread, prop_msg> Logger(x);

    CLOG(Logger, ERROR) << "broke down with error code " << 16;

    return (x.str().find("0x") != std::string::npos && // prints thread in hex
            x.str().find("16") != std::string::npos); // prints following in dec
  })();

  test::make("Prints the same hash for identical object references", []() {
    // Logger setup
    std::ostringstream x, y;
    logger::Logger<DEBUG, decltype(x), my_format, prop_hash> Logger(x),
        Logger2(y);

    // print an instance twice, so that the hashes are the same
    std::string msg = "test message with one address";
    CLOG(Logger, ERROR) << msg;
    CLOG(Logger2, ERROR) << msg;

    return (x.str().find("0x") != std::string::npos && x.str() == y.str());
  })();

  test::make("Prints different hashes for different object refs", []() {
    // Logger setup
    std::ostringstream x, y;
    logger::Logger<DEBUG, decltype(x), my_format, prop_hash> Logger(x),
        Logger2(y);

    // print the same message but from different objects; different hashes
    std::string msg = "test message with one address";
    x.clear(); y.clear();
    std::string msg2 = msg;
    CLOG(Logger, ERROR) << msg2;
    CLOG(Logger2, ERROR) << msg;

    return (x.str().find("0x") != std::string::npos && x.str() != y.str());
  })();

  test::make("Hashes should depend only on enabled msg. segments", []() {
    // Logger setup
    std::ostringstream x, y;
    logger::Logger<DEBUG, decltype(x), my_format, prop_hash> Logger(x),
        Logger2(y);

    // output twice with differing hashes in ignored areas
    std::string msg = "hello ", msg2 = msg;
    CLOG(Logger, ERROR) << logger::hash::off << msg2 << logger::hash::on << msg;
    CLOG(Logger2, ERROR) << logger::hash::off << msg << logger::hash::on << msg;

    return (x.str().find("0x") != std::string::npos && x.str() == y.str());
  })();

  test::make("Prints the proper context of the log call", []() {
    // Logger setup
    std::ostringstream x;
    logger::Logger<DEBUG, decltype(x), my_format, prop_line, prop_file,
                   prop_func>
        Logger(x);

    CLOG(Logger, ERROR) << "test";
    auto l = __LINE__ - 1;
    auto f = __FILE__, n = __func__;

    return (x.str().find(f) != std::string::npos &&
            x.str().find(n) != std::string::npos &&
            x.str().find(std::to_string(l)) != std::string::npos);
  })();
}

void test_format() {
  using namespace logger;
  test::make("Basic logger prints naked messages", []() {
    std::ostringstream x;
    BasicLogger<std::ostringstream> Logger(x);

    CLOG(Logger, ERROR) << "broke down";
    return (x.str() == "broke down\n");
  })();

  test::make("Doesn't print out properties after the last wildcard", []() {
    std::ostringstream x;
    Logger<DEBUG, std::ostringstream, basic_fmt, prop_hash,
                   prop_msg>
        Logger(x);

    CLOG(Logger, ERROR) << "broke down";
    return (x.str().find("broke down") == std::string::npos);
  })();

  test::make("Wildcards not associated with property printed raw", []() {
    std::ostringstream x;
    Logger<DEBUG, std::ostringstream, basic_fmt> Logger(x);

    CLOG(Logger, ERROR) << "broke up";
    return (x.str() == "%\n");
  })();
}


int main() {
  test_basic();
  test_props();
  test_format();

  // TODO: test when #% exceeds arguments
  // TODO: test when arguments exceeds #%

  return 0;
}
