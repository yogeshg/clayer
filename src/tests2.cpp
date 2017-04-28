#include "property.h"
#include "analyser.h"
#include "logconfig.h"
#include "util.h"
#include <regex>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <map>

using namespace clayer;

bool contains(std::string str, std::string substr) {
    return str.find(substr)!=std::string::npos;
}

// Single Property correctly read and written
bool t1() {
    std::cout << "testing " << __func__ << "\n";
    std::vector<std::string> v {"Hello", "World", "123"};
    LogRecord lr;
    read_prop<log_properties::DATE>(lr, v[0]);
    read_prop<log_properties::FUNC>(lr, v[1]);
    read_prop<log_properties::LINE>(lr, v[2]);
    std::stringstream ss;
    get_props<log_properties::DATE, log_properties::FUNC, log_properties::LINE>(ss, lr);
    const std::string str = ss.str();
    return contains(str, v[0]) & contains(str, v[1]) & contains(str, v[2]);
}

// Multiple Properties correctly read and written
bool t2() {
    std::cout << "testing " << __func__ << "\n";
    std::vector<std::string> v {"Hello", "World", "123"};
    LogRecord lr;
    read_props<std::vector<std::string>::iterator, log_properties::DATE, log_properties::FUNC, log_properties::LINE>(lr, v.begin());
    std::stringstream ss;
    get_props<log_properties::DATE, log_properties::FUNC, log_properties::LINE>(ss, lr);
    const std::string str = ss.str();
    return contains(str, v[0]) & contains(str, v[1]) & contains(str, v[2]);
}

template <typename T>
bool t3(){
    std::cout << "testing " << __func__ << "\n";
    T container;
    // {"Hello", "World", "123"};
    std::vector<std::string> v {"Hello", "World", "123"};
    for (int i = 0; i < v.size(); ++i)
        container.emplace(i, v[i]);
    std::stringstream ss;
    util::to_string(ss, container.begin(), container.end());
    const std::string str = ss.str();
    return contains(str, v[0]) & contains(str, v[1]) & contains(str, v[2]);
}

int main () {
    assert( t1() ) ;
    assert( t2() ) ;
    {const int o1 = t3<std::map<int, std::string>>(); assert( o1 ) ;}
    {const int o1 = t3<std::unordered_map<int, std::string>>(); assert( o1 ) ;}
    return 0;
}