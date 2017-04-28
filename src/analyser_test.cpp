#include "analyser.h"
#include "logconfig.h"
#include "property.h"
#include <regex>

using namespace clayer;

int run(std::string filename) {

  analyser::Parser parser{};

  // [2017-04-27 11:29:05] DEBUG[Thread 0x7fff7317a310:src/runner.cpp(main:23)]:
  // [Begin logging]
  std::regex log_format(".*\\[(.*) (.*)\\].* (.*)\\[Thread "
                        "(.*):(.*)\\((.*):(.*)\\)\\]: \\[(.*)\\] \\[(.*)\\]");

  auto recs =
      parser.read_file<clayer::DATE, clayer::TIME, clayer::LEVEL,
                       clayer::THREAD, clayer::FILE, clayer::FUNC, clayer::LINE,
                       clayer::MESG, clayer::HASH>(filename, log_format);

  // print ten lines
  std::cout << "ten lines : \n";
  int head = 10;
  for (auto r : recs) {
    std::cout << r << std::endl;
    if (!head--)
      break;
  }

  // print all states
  std::cout << "all states : \n";
  auto states = parser.get_states();
  util::to_string(std::cout, states.begin(), states.end(), "\n") << std::endl;

  // print stats by file name
  std::cout << "stats by file name : \n";
  auto file_stats =
      analyser::DomainStat<log_properties::FILE>(recs, 1.0, 3, 0.1);
  std::cout << file_stats << std::endl;

  // print stats by file and function names
  std::cout << "stats by file and function names : \n";
  auto func_stats =
      analyser::DomainStat<log_properties::FILE, log_properties::FUNC>(recs, 10,
                                                                       7, 0.2);
  std::cout << func_stats << std::endl;

  // print frequency distribution of deposit function, balance number
  std::cout
      << "frequency distribution of deposit function, balance number : \n";
  const auto fd1 = func_stats.domain_stats["deposit"].stats[0].freq_dist;
  util::to_string(std::cout, fd1.begin(), fd1.end()) << std::endl;

  // print frequency distribution of withdraw function, balance number
  std::cout
      << "frequency distribution of withdraw function, balance number : \n";
  const auto fd2 = func_stats.domain_stats["withdraw"].stats[0].freq_dist;
  util::to_string(std::cout, fd2.begin(), fd2.end()) << std::endl;

  // print stats by thread names
  std::cout << "stats by thread names : \n";
  auto thread_stat = analyser::DomainStat<log_properties::THREAD>(recs);
  std::cout << thread_stat << std::endl;

  return 0;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "usage:\n\t" << argv[0] << " <logfilename>\n";
    return 1;
  }
  run(argv[1]);
  return 0;
}
