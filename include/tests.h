/**
 * @brief Tiny test suite application that associates test procedures with
 * labels and reports results of the test using our logger.
 */
#include <iostream>
#include <type_traits>

#include "logconfig.h"
#include "logger.h"

namespace test {
using namespace logger;

enum Status { PASS, FAIL };

/**
 * @brief A custom Prop that prints out the fraction of passing tests so far by
 * keeping an internal counter.
 */
template <typename Stream> void prop_testno(Stream &o, const Line &l) {
  static int test_no = 0;
  static int success_no = 0;
  if (l.info.level == PASS)
    ++success_no;
  if (l.info.level == PASS || l.info.level == FAIL)
    ++test_no;
  o << "\033[1m" << success_no << "/" << test_no << "\033[0m";
}

/**
 * @brief A custom Prop that repurposes the info level to report the success
 * status of the test associated with the log.
 */
template <typename Stream> void prop_stat(Stream &o, const Line &l) {
  switch (l.info.level) {
  case PASS:
    o << "\033[1;32mPASS\033[0m";
    break;
  case FAIL:
    o << "\033[1;31mFAIL\033[0m";
    break;
  default:
    o << "\033[1;37m????\033[0m";
    break;
  }
}

/**
 * @brief Format and corresponding logger TestLogger for test output.
 */
constexpr const char min_fmt[] = R"([%] % - "%")";
Logger<std::ostream, NOTSET, min_fmt, prop_testno, prop_stat, prop_msg>
    TestLogger(std::cout);

template <typename T> concept bool Testable = requires(T t) {
  { t() }->bool;
  {std::is_convertible<decltype(t()), bool>::value == true};
};

/**
 * @brief A simple class to represent a unit test.
 */
template <Testable F> class Test {
  /**
   * @brief A description of the test.
   */
  std::string desc_;

  /**
   * @brief A runnable test procedure.
   */
  F test_;

public:
  /**
   * @brief Constructs the test from a description and a procedure.
   *
   * @param desc The description of the test.
   * @param test The procedure to run for the test.
   */
  Test(const std::string &desc, const F &test) : desc_(desc), test_(test){};

  /**
   * @brief Runs the test and reports using our logger if it passed or failed.
   */
  void operator()() {
    if (test_())
      CLOGL(TestLogger, PASS) << desc_;
    else
      CLOGL(TestLogger, FAIL) << desc_;
  }
};

/**
 * @brief Helper function for creating a test without specifying the procedure
 * type.
 *
 * @param desc The test description.
 * @param test The test procedure.
 */
template <Testable F> Test<F> make(const std::string &desc, const F &test) {
  return Test<F>(desc, test);
}
} // namespace test
