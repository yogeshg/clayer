#ifndef __ANALYSER_H__
#define __ANALYSER_H__

#include "property.h"
#include <fstream>
#include <set>

namespace clayer {
    namespace analyser {

        class Parser {
            std::vector<LogRecord> records;
        public:

            const std::vector<LogRecord> read_file(std::string filename) {
                records.clear();
                std::ifstream f(filename);
                for(std::string line; std::getline(f, line); ) {
                    LogRecord p;
                    // std::cout << p <<" -- " << line << "\n";
                    parse_props<clayer::FILE, clayer::FUNC, clayer::LINE, clayer::MESG>(p, line);
                    // std::cout << p <<" -- " << line << "\n";
                    records.push_back(p);
                }
                return records;
            }

            std::set<LogRecord::State> get_states() {
                std::set<LogRecord::State> states;
                for(auto r : records) {
                    states.insert(r.get_state());
                }
                return states;
            }
        };

    }
}

#endif /*__ANALYSER_H__*/
