#ifndef __ANALYSER_H__
#define __ANALYSER_H__

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include "property.h"

namespace clayer {
namespace analyser {



class Parser {
  std::vector<LogRecord> records;

public:
  template <log_properties... I>
  const std::vector<LogRecord> read_file(std::string filename, std::regex log_format) {
    records.clear();
    std::ifstream f(filename);
    for (std::string line; std::getline(f, line);) {
      LogRecord p;
      // std::cout << p <<" -- " << line << "\n";
      parse_props<I...>(p, line, log_format);
      // parse_props<FILE, FUNC, LINE, MESG>(p, line);

      // std::cout << p <<" -- " << line << "\n";
      records.push_back(p);
    }
    return records;
  }

  std::set<LogRecord::State> get_states() {
    std::set<LogRecord::State> states;
    for (auto r : records) {
      states.insert(r.get_state());
    }
    return states;
  }
};

template <log_properties p> class DomainStat {
  // TODO1: currently taking a function scope as a domain, want to generalize it
  // to FILE, DATE...
  // TODO2: v1.2 consider function hierachy
  std::vector<LogRecord> records_to_analyze;
  std::map<std::string, int> domain_stats;

public:
  // TODO3: expand this to incorporate more statistics, such as count by levels

  DomainStat() = default;

  DomainStat(std::vector<LogRecord> records) : records_to_analyze(records) {
    for (auto record : records) {
      auto log_stat = record.get_state();
      std::string domain_name = domain_abstraction(log_stat);
      if (domain_stats.find(domain_name) == domain_stats.end()) {
        // not found
        domain_stats[domain_name] = 1;
      } else {
        // found
        domain_stats[domain_name] += 1;
      }
    }
  }

  std::string domain_abstraction(CodeContext state) {
    switch (p) {
    case log_properties::FILE:
      return state.file;
    case log_properties::FUNC:
      return state.file+" "+state.func;
    case log_properties::LINE:
      return state.file+" "+state.func+":"+std::to_string(state.line);
    default:
      return state.file;
    }
  }

  std::ostream &print_domain_stats(std::ostream &s) {
    // iterate thru the map and print the count
    for (auto domain_stat : domain_stats) {
      s << "Inside \"" << domain_stat.first << "\": " << domain_stat.second
        << " logs captured" << std::endl;
    }
    return s;
  }
};
std::ostream &operator<<(std::ostream &s, auto stat) {
  return stat.print_domain_stats(s);
}
};
}

#endif /*__ANALYSER_H__*/
