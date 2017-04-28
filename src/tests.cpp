/**
 * Unit tests for the logging library, with help from the library itself for
 * reporting results.
 * 
 * Also demonstrates a nontrivial use of custom Props and custom formats.
 */
#include "tests.h"

#include "analyser.h"
#include "logconfig.h"
#include "logger.h"
#include "property.h"
#include "util.h"

#include <map>
#include <regex>
#include <unordered_map>
#include <vector>

/**
 * Helper function for string tests.
 */
static bool contains(std::string str, std::string substr) {
    return str.find(substr)!=std::string::npos;
}

namespace logger {
constexpr const char my_format[] = "[%] %[%:%(%:%)]: [%]";
using MyLogger =
    Logger<std::ostringstream, DEBUG, my_format, prop_time, prop_level,
           prop_thread, prop_file, prop_func, prop_line, prop_msg>;
}

/**
 * @brief Basic sanity checks for the logging library and static thresholds.
 */
void test_basic() {
  test::make("Prints the message normally", []() {
    std::ostringstream x;
    logger::BasicLogger<std::ostringstream, logger::INFO> Logger(x);

    CLOG(Logger, ERROR) << "broke down";
    auto p = x.str().find("broke down");

    return (p != std::string::npos);
  })();

  test::make("Prints multiple messages in order", []() {
    std::ostringstream x;
    logger::BasicLogger<std::ostringstream, logger::INFO> Logger(x);

    CLOG(Logger, ERROR) << "broke up";
    CLOG(Logger, WARNING) << "broke down";
    auto p = x.str().find("broke up");

    return (p != std::string::npos &&
            x.str().find("down", p + 1) != std::string::npos);
  })();

  test::make("Prints nothing when threshold too high", []() {
    std::ostringstream x;
    logger::BasicLogger<std::ostringstream, logger::INFO> Logger(x);

    CLOG(Logger, DEBUG) << "just telling you";

    return x.str().empty();
  })();

  test::make("Should filter out messages below threshold", []() {
    std::ostringstream x;
    logger::BasicLogger<std::ostringstream, logger::INFO> Logger(x);

    CLOG(Logger, WARNING) << "serious";
    CLOG(Logger, NOTSET) << "funny";

    return (x.str().find("serious") != std::string::npos &&
            x.str().find("funny") == std::string::npos);
  })();
}

/**
 * @brief Tests for correctness of the Prop functions that output contextual
 * information.
 */
void test_props() {
  using namespace logger;
  test::make("Can omit messages if not in format", []() {
    std::ostringstream x;
    logger::Logger<std::ostringstream, DEBUG, basic_fmt, prop_level> Logger(x);

    CLOG(Logger, ERROR) << "broke down";

    return (x.str().find("broke down") == std::string::npos);
  })();

  test::make("Correctly prints preset log level strings", []() {
    std::ostringstream x;
    logger::Logger<std::ostringstream, DEBUG, basic_fmt, prop_level> Logger(x);

    CLOG(Logger, ERROR) << "broke down";
    CLOG(Logger, WARNING) << "broke down again";

    return (x.str().find("ERROR") != std::string::npos &&
            x.str().find("WARNING") != std::string::npos);
  })();

  test::make("Correctly prints thread while preserving number format", []() {
    std::ostringstream x;
    logger::Logger<std::ostringstream, DEBUG, my_format, prop_thread, prop_msg> Logger(x);

    CLOG(Logger, ERROR) << "broke down with error code " << 16;

    return (x.str().find("0x") != std::string::npos && // prints thread in hex
            x.str().find("16") != std::string::npos); // prints following in dec
  })();

  test::make("Prints the same hash for identical object references", []() {
    // Logger setup
    std::ostringstream x, y;
    logger::Logger<decltype(x), DEBUG, my_format, prop_hash> Logger(x),
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
    logger::Logger<decltype(x), DEBUG, my_format, prop_hash> Logger(x),
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
    logger::Logger<decltype(x), DEBUG, my_format, prop_hash> Logger(x),
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
    logger::Logger<decltype(x), DEBUG, my_format, prop_line, prop_file,
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

/**
 * @brief Tests for correctness of the formatting and substitution procedure.
 */
void test_format() {
  using namespace logger;
  test::make("Basic logger prints naked messages", []() {
    std::ostringstream x;
    logger::BasicLogger<std::ostringstream, logger::INFO> Logger(x);

    CLOG(Logger, ERROR) << "broke down";
    return (x.str() == "broke down\n");
  })();

  test::make("Doesn't print out properties after the last wildcard", []() {
    std::ostringstream x;
    Logger<std::ostringstream, DEBUG, basic_fmt, prop_hash,
                   prop_msg>
        Logger(x);

    CLOG(Logger, ERROR) << "broke down";
    return (x.str().find("broke down") == std::string::npos);
  })();

  test::make("Wildcards not associated with property printed raw", []() {
    std::ostringstream x;
    Logger<std::ostringstream, DEBUG, basic_fmt> Logger(x);

    CLOG(Logger, ERROR) << "broke up";
    return (x.str() == "%\n");
  })();

  test::make("Filters appropriate reject log messages", []() {
    std::ostringstream x;
    Logger<std::ostringstream, DEBUG, basic_fmt, prop_msg> Logger(x);

    // Filter out strings that contain "g"
    Logger.set_filter([](auto &l) {
      return (l.message.str().find("g") == std::string::npos);
    });

    CLOG(Logger, ERROR) << "bad";
    CLOG(Logger, ERROR) << "good";
    return (x.str() == "bad\n");
  })();
}

void test_analyse() {
  using namespace clayer;
  test::make("Single Property correctly read and written", []() {
    std::vector<std::string> v{"Hello", "World", "123"};
    LogRecord lr;
    read_prop<log_properties::DATE>(lr, v[0]);
    read_prop<log_properties::FUNC>(lr, v[1]);
    read_prop<log_properties::LINE>(lr, v[2]);
    std::stringstream ss;
    get_props<log_properties::DATE, log_properties::FUNC, log_properties::LINE>(
        ss, lr);
    const std::string str = ss.str();
    return contains(str, v[0]) && contains(str, v[1]) && contains(str, v[2]);
  })();

  test::make("Multiple properties correctly read and written", []() {
    std::vector<std::string> v{"Hello", "World", "123"};
    LogRecord lr;
    read_props<std::vector<std::string>::iterator, log_properties::DATE,
               log_properties::FUNC, log_properties::LINE>(lr, v.begin());
    std::stringstream ss;
    get_props<log_properties::DATE, log_properties::FUNC, log_properties::LINE>(
        ss, lr);
    const std::string str = ss.str();
    return contains(str, v[0]) && contains(str, v[1]) && contains(str, v[2]);
  })();

  test::make("Maps correctly printed", []() {
    std::map<int, std::string> container;
    std::vector<std::string> v {"Hello", "World", "123"};
    for (unsigned int i = 0; i < v.size(); ++i)
        container.emplace(i, v[i]);
    std::stringstream ss;
    util::to_string(ss, container.begin(), container.end());
    const std::string str = ss.str();
    return contains(str, v[0]) && contains(str, v[1]) && contains(str, v[2]);
  })();

  test::make("Unordered Maps correctly printed", []() {
    std::unordered_map<int, std::string> container;
    std::vector<std::string> v {"Hello", "World", "123"};
    for (unsigned int i = 0; i < v.size(); ++i)
        container.emplace(i, v[i]);
    std::stringstream ss;
    util::to_string(ss, container.begin(), container.end());
    const std::string str = ss.str();
    return contains(str, v[0]) && contains(str, v[1]) && contains(str, v[2]);
  })();

  test::make("Single Stat correctly printed", []() {
    util::Stat<float> s1;
    std::stringstream ss;

    s1.add(1);
    s1.add(2);

    ss << s1 << "\n";
    std::string str;
    str = ss.str();

    return contains(str, "1") && contains(str, "2") && contains(str, "1.5");
  })();

  test::make("Multiple Stat correctly calculated", []() {
    std::vector<float> n1 {-100, 5.32, 24};
    std::vector<std::vector<float>> n2(28, {2, 6.32, 25});

    util::VectorStat<float> s2{};
    s2.add(n1);
    for (auto n: n2) {
        s2.add(n);
    }

    return (s2.stats[0].min <= n1[0] &&
            s2.stats[0].max >= n1[0] &&
            (unsigned int)(s2.stats[0].num) == 1 + n2.size());
  })();
}

int main() {
  test_basic();
  test_props();
  test_format();
  test_analyse();

  return 0;
}
