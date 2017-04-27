#include "property.h"
#include "analyser.h"
#include "logconfig.h"
#include <regex>

using namespace clayer;

int run(std::string filename) {

  analyser::Parser parser{};

  // [2017-04-27 11:29:05] DEBUG[Thread 0x7fff7317a310:src/runner.cpp(main:23)]: [Begin logging]
  std::regex format_regex("\\[.*\\] (.*)\\[.*\\]: \\[.*\\]");

  auto recs = parser.read_file<clayer::FILE, clayer::FUNC, clayer::LINE, clayer::MESG>(filename, format_regex);

  for (auto r : recs) {
    std::cout << r << std::endl;
  }

  auto states = parser.get_states();
  for (auto s : states) {
    std::cout << s << std::endl;
  }

  auto domain_stats = analyser::DomainStat<log_properties::FILE>(recs);
  // domain_stats.print_domain_stats();
  std::cout << domain_stats << std::endl;
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
