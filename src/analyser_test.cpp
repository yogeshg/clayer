#include "logconfig.h"
#include "property.h"
#include "analyser.h"

using namespace clayer;

int run(std::string filename) {

    analyser::Parser parser {};

    auto recs = parser.read_file(filename);
    for(auto r : recs) {
        std::cout << r << std::endl;
    }

    auto states = parser.get_states();
    for(auto s : states) {
        std::cout << s << std::endl;
    }

    return 0;
}

int main(int argc, char** argv) {

  if(argc < 2) {
    std::cout << "usage:\n\t" << argv[0] << " <logfilename>\n";
    return 1;
  }

  run(argv[1]);

  return 0;
}
