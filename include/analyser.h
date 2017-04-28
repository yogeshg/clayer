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

std::vector<float> get_numbers(const std::string &line,
                               const int maxtokens = 100) {
  std::vector<float> numbers;
  std::istringstream is(line);
  float val = 0;
  std::string s;
  for (int i = 0; i < maxtokens; ++i) {
    is >> val;
    if (is.fail()) {
      is.clear();
      is >> s;
      if (is.fail()) {
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
  const std::vector<LogRecord> read_file(std::string filename,
                                         std::regex log_format) {
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

template <log_properties... p> class DomainStat {
  // TODO1: currently taking a function scope as a domain, want to generalize it
  // to FILE, DATE...
  // TODO2: v1.2 consider function hierachy
  std::vector<LogRecord> records_to_analyze;

public:
  std::map<std::string, util::VectorStat<float>> domain_stats;

  DomainStat() = default;

  DomainStat(std::vector<LogRecord> records, float precision = 1.0,
             int outlier_count = 10, float outlier_fraction = 0.01)
      : records_to_analyze(records) {
    for (auto record : records) {
      auto log_stat = record.get_state();
      std::stringstream ss;
      get_props<p...>(ss, record);
      std::string domain_name = ss.str();

      if (domain_stats.find(domain_name) == domain_stats.end()) { // not found
        domain_stats.emplace(domain_name,
                             util::VectorStat<float>(precision, outlier_count,
                                                     outlier_fraction));
      }
      domain_stats[domain_name].add(record.numbers);
    }
  }

  std::ostream &to_string(std::ostream &s) {
    util::to_string(s, domain_stats.begin(), domain_stats.end(), ",\n");
    return s;
  }
};
std::ostream &operator<<(std::ostream &s, auto stat) {
  return stat.to_string(s);
}
}; // namespace analyser
} // namespace clayer

#endif /*__ANALYSER_H__*/
