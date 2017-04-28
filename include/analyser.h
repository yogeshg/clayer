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

std::vector<float> get_numbers (const std::string& line, const int maxtokens=100) {
    std::vector<float> numbers;
    std::istringstream is(line);
    float val=0;
    std::string s;
    for(int i=0; i<maxtokens; ++i) {
        is >> val;
        if(is.fail()) {
            is.clear();
            is >> s;
            if(is.fail()) {
                break;
            }
            // ignored tokens here
            // std::cout << i << ":" << s <<"\n";
        } else {
            numbers.push_back(val);
        }
    }
    return numbers;
}


class Parser {
  std::vector<LogRecord> records;

public:
  template <log_properties... I>
  const std::vector<LogRecord> read_file(std::string filename, std::regex log_format) {
    records.clear();
    std::ifstream f(filename);
    for (std::string line; std::getline(f, line);) {
      LogRecord p;
      parse_props<I...>(p, line, log_format);
      p.numbers = get_numbers(p.message);
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
  std::map<std::string, util::VectorStat<float>> domain_stats2;

public:
  // TODO3: expand this to incorporate more statistics, such as count by levels

  DomainStat() = default;

  DomainStat(std::vector<LogRecord> records, float precision=1.0, int outlier_count=10, float outlier_fraction=0.01) :
    records_to_analyze(records) {
    for (auto record : records) {
      auto log_stat = record.get_state();
      // record.numbers;
      std::string domain_name = domain_abstraction(log_stat);
      if (domain_stats.find(domain_name) == domain_stats.end()) { // not found
        domain_stats[domain_name] = 0;
      }
      domain_stats[domain_name] += 1;

      if (domain_stats2.find(domain_name) == domain_stats2.end()) { // not found
        domain_stats2.emplace(domain_name, util::VectorStat<float>(precision, outlier_count, outlier_fraction));
      }
      domain_stats2[domain_name].add(record.numbers);
    }
  }

  std::string domain_abstraction(LogRecord::State &state) {
    CodeContext &c = state.first;
    RunContext &r = state.second;

    switch (p) {
    case log_properties::FILE:
      return c.file;
    case log_properties::FUNC:
      return c.file+" "+c.func;
    case log_properties::LINE:
      return c.file+" "+c.func+":"+std::to_string(c.line);
    case log_properties::DATE:
      return r.date;
    case log_properties::TIME:
      return r.time;
    case log_properties::LEVEL:
      return c.level;
    case log_properties::THREAD:
      return r.thread;
    default:
      return c.level;
    }
  }

  std::ostream &to_string(std::ostream &s) {
    // iterate thru the map and print the count
    // for (auto domain_stat : domain_stats2) {
    //   s  << domain_stat.first << " : " << domain_stat.second
    //   << std::endl;
    // }
    util::to_string(s, domain_stats2.begin(), domain_stats2.end(), ",\n");
    return s;
  }
};
std::ostream &operator<<(std::ostream &s, auto stat) {
  return stat.to_string(s);
}
};
}

#endif /*__ANALYSER_H__*/
