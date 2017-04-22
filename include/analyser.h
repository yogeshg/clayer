#ifndef __ANALYSER_H__
#define __ANALYSER_H__

#include "property.h"
#include <fstream>
#include <set>

namespace clayer {
    namespace analyser {

        class Parser {
            std::vector<Property> records;
        public:

            const std::vector<Property> read_file(std::string filename) {
                records.clear();
                std::ifstream f(filename);
                for(std::string line; std::getline(f, line); ) {
                    Property p;
                    // std::cout << p <<" -- " << line << "\n";
                    parse_props<clayer::FILE, clayer::FUNC, clayer::LINE>(p, line);
                    // std::cout << p <<" -- " << line << "\n";
                    records.push_back(p);
                }
                return records;
            }

            std::set<Property::State> get_states() {
                std::set<Property::State> states;
                for(auto r : records) {
                    states.insert(r.get_state());
                }
                return states;
            }
        };

    }
}

#endif /*__ANALYSER_H__*/
