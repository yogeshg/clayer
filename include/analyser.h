/**
 * Class and function definitions to perform analysis on raw logging output
 */

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

  /**
   * @brief Parse a string and given the maximum number of tokens, return a vector
   * containing all the numbers inside that line.
   *
   * @param line The string we want the function to parse.
   *
   * @param maxtoken Maximum number of tokens to parse.
   *
   * @return a vector containing float numbers extracted from line.
   */
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


/**
 * @brief Parses a log file into log records and also provides a set of states if required
 */
class Parser {
  std::vector<LogRecord> records;

public:
  /**
   * @brief Reads log properties from a file according to the log format regex supplied
   * @param filename name of the file to read from
   * @param log_format the regular expression to match the line with; it should
   * contain as many matching groups (excluding the default group) as there are
   * properties in the template argument; each group will be parsed into the
   * respective property
   * @return const reference to the records thus read
   */
  template <log_properties... I>
  const std::vector<LogRecord>& read_file(std::string filename, std::regex log_format) {
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

  /**
   * @brief creates a set of states identified in the log records
   * @return the set thus created
   */
  std::set<LogRecord::State> get_states() {
    std::set<LogRecord::State> states;
    for (auto r : records) {
      states.insert(r.get_state());
    }
    return states;
  }
};

/**
 * @brief This is a class that provides querying functionalities on LogRecord objects.
 * We construct a DomainStat object based on a series of log_properties, taken in
 * template arguments and group by the LogRecord objects based on such log_properties.
 */
template <log_properties... p> class DomainStat {
  // TODO1: currently taking a function scope as a domain, want to generalize it
  // to FILE, DATE...
  // TODO2: v1.2 consider function hierachy

  /**
   * @brief A private vector containing LogRecords objects that DomainStat will
   * perform analysis on.
   */
  std::vector<LogRecord> records_to_analyze;

public:
  /**
   * @brief This is a map that maps from a certain log_property typed string
   * to a VectorStat object containing floats. We maintain this map to keep the
   * result of the analysis.
   */
  std::map<std::string, util::VectorStat<float>> domain_stats;

  /**
   * @brief This is the default constructor that does nothing.
   */
  DomainStat() = default;

  /**
   * @brief This constuctor takes in a vector of LogRecord objects to perform
   * analysis on, given the precision of the statistical analyser, the maximum
   * count of ourliers that it identifies and the corresponding proportion of
   * the ourliers.
   *
   * @param records A vector containing LogRecord objects that DomainStat class
   * will perform analysis on.
   *
   * @param precision The precision factor that the statistical analyser relies on.
   *
   * @param outlier_count Maximum number of outliers that DomainStat will identify
   * from the records.
   *
   * @param outlier_fraction Maximum proportion of outliers that DomainStat will
   * identify from the records.
   */
  DomainStat(std::vector<LogRecord> records,
      float precision=1.0, int outlier_count=10, float outlier_fraction=0.01) :
    records_to_analyze(records) {
    for (auto record : records) {
      auto log_stat = record.get_state();
      std::stringstream ss;
      get_props<p...>(ss, record);
      std::string domain_name = ss.str();

      if (domain_stats.find(domain_name) == domain_stats.end()) { // not found
        domain_stats.emplace(domain_name, util::VectorStat<float>(precision, outlier_count, outlier_fraction));
      }
      domain_stats[domain_name].add(record.numbers);
    }
  }

    /**
     * @brief A helper function that prints a DomainStat objects to
     * the output ostream.
     *
     * @param s A std::ostream object that DomainStat will print to.
     *
     * @return std::ostream object that the function takes in.
     */
  std::ostream &to_string(std::ostream &s) {
    util::to_string(s, domain_stats.begin(), domain_stats.end(), ",\n");
    return s;
  }
};

/**
 * @brief Stream out operator overwrite to allow convinent extraction of
 * the result that DomainStat obtained from its analysis.
 *
 * @param s An std::ostream typed stream object to print to
 *
 * @param stat A DomainStat object to print.
 *
 * @return std::ostream object that the function takes in.
 */
std::ostream &operator<<(std::ostream &s, auto stat) {
  return stat.to_string(s);
}
};
}

#endif /*__ANALYSER_H__*/
