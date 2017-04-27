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
  const std::vector<LogRecord> read_file(std::string filename) {
    records.clear();
    std::ifstream f(filename);
    for (std::string line; std::getline(f, line);) {
      LogRecord p;
      // std::cout << p <<" -- " << line << "\n";
      parse_props<clayer::DATE, clayer::TIME, clayer::LEVEL, clayer::THREAD, clayer::FILE, clayer::FUNC, clayer::LINE, clayer::MESG>(p, line);
      // std::cout << p <<" -- " << line << "\n";
      records.push_back(p);
    }
    return records;
  }

  std::vector<LogRecord::State> get_states() {
    std::vector<LogRecord::State> states;
    for (auto r : records) {
      states.push_back(r.get_state());
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

  //  static std::string DomainSep(";");

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
  
  std::string domain_abstraction(std::pair<CodeContext, RunContext> &state) {
    // TODO: we have more runtime domains to identify
    CodeContext &c = state.first;
    RunContext &r = state.second;
    switch (p) {
    case log_properties::FILE:
      return c.file;
    case log_properties::FUNC:
      return c.file+" "+c.func;
    case log_properties::LINE:
      return c.file+" "+c.func+" "+std::to_string(c.line);
    case log_properties::DATE:
      return r.date;
    case log_properties::TIME:
      return r.time;
    case log_properties::LEVEL:
      return r.level;
    case log_properties::THREAD:
      return r.thread;
    default:
      return r.level;
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
