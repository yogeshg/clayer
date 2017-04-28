#include "property.h"
#include "analyser.h"
#include "logconfig.h"
#include <regex>

using namespace clayer;

int run(std::string filename) {

  analyser::Parser parser{};

  // [2017-04-27 11:29:05] DEBUG[Thread 0x7fff7317a310:src/runner.cpp(main:23)]: [Begin logging]
  std::regex log_format(".*\\[(.*) (.*)\\].* (.*)\\[Thread (.*):(.*)\\((.*):(.*)\\)\\]: \\[(.*)\\] \\[(.*)\\]");

  auto recs = parser.read_file<clayer::DATE, clayer::TIME,
                                clayer::LEVEL,
                                clayer::THREAD, clayer::FILE,
                                clayer::FUNC, clayer::LINE,
                               clayer::MESG, clayer::HASH>(filename, log_format);

  for (auto r : recs) {
    std::cout << r << std::endl;
    break;
  }

  auto states = parser.get_states();
  for (auto s : states) {
    std::cout << s << std::endl;
  }

  auto file_stats = analyser::DomainStat<log_properties::FILE>(recs, 1.0, 3, 0.001);
  std::cout << "file_stats : \n" << file_stats << std::endl;

  auto func_stats = analyser::DomainStat<log_properties::FUNC>(recs, 1.0, 7, 0.001);
  std::cout << "func_stats : \n" << func_stats  << std::endl;

  auto thread_stat = analyser::DomainStat<log_properties::THREAD>(recs);
  std::cout << "thread_stat : \n" << thread_stat << std::endl;

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
