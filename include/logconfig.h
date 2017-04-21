#ifndef __LOGCONFIG_H__
#define __LOGCONFIG_H__

#include <regex>

namespace logger {
constexpr int THRESHOLD = 0;
constexpr char filename[] = "hey.txt";
constexpr char format[] = "%(%:%):";
std::regex format_regex = std::regex("(.*)\\((.*):(.*)\\):(.*)");
// using MyFormat = Format(date, line, ip);

// Format("(%s:%s:%s)", date, line, ip);
// constexpr FORMAT
}

#endif /* __LOGCONFIG_H__ */