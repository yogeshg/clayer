#include <iostream>
#include <thread>

#include "logconfig.h"
#include "logger.h"
#include "analyser.h"
#include <string>

int run(std::string filename) {
  analyser::Parser<logger::format> parser;

  parser.read_file( filename );
  auto states = parser.get_states();
  for(auto s : states) {
    std::cout << s << std::endl;
  }

}

int main(int argc, char** argv) {

  if(argc < 2) {
    std::cout << "usage:\n\t" << argv[0] << " <logfilename>\n";
    return 1;
  }

  run(argv[1]);

  return 0;
}
