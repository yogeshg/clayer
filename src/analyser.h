#ifndef __ANALYSER_H__
#define __ANALYSER_H__

#include "logger.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>

namespace analyser {

    struct ContextInfo {
      std::string file, fn;
      int line;
      std::tuple<std::string,std::string,int> tie() const{
        return std::tie(file, fn, line);
      }
      bool operator<(const ContextInfo& rhs) const {
        return tie() < rhs.tie();
      }
    };

    static std::string CONTEXT_INFO_SEP (" ");
    std::ostream& operator<<(std::ostream& s, ContextInfo c) {
        return s << c.file <<  CONTEXT_INFO_SEP << c.fn <<  CONTEXT_INFO_SEP << c.line;
    }

    struct ParsedRecord {
        using State = ContextInfo;
        ContextInfo info;
        std::string message;

        ParsedRecord(std::string& line) {
            std::smatch m;
            std::regex_match ( line, m, logger::format_regex );
            auto it = m.begin();
            ++it;
            info.file = *it;
            it++;
            info.fn = *it;
            it++;
            it++;
            message = *it;
        }

        State get_state() {
            return info;
        }
    };

    template <const char *Fmt> class Parser {
        std::vector<ParsedRecord> records;
    public:

        const std::vector<ParsedRecord> read_file(std::string filename) {
            records.clear();
            std::ifstream f(filename);
            for(std::string line; std::getline(f, line); ) {
                ParsedRecord p(line);
                // std::cout << p.info.file << p.info.fn << p.info.line << p.message << std::endl;
                records.push_back(p);
            }
            return records;
        }

        std::set<ParsedRecord::State> get_states() {
            std::set<ParsedRecord::State> states;
            for(auto r : records) {
                states.insert(r.get_state());
            }
            return states;
        }


    };
}

#endif/*__ANALYSER_H__*/
