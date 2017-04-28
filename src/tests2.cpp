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

// Maps correctly printed
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


// Single Stat correctly calculated
bool t4() {
    std::cout << "testing " << __func__ << "\n";

    util::Stat<float> s1;
    std::stringstream ss;

    s1.add(1);
    // std::cout << s1 << "\n";

    s1.add(2);
    // std::cout << s1 << "\n";

    ss << s1 << "\n";
    std::string str;
    str = ss.str();

    return contains(str, "1") & contains(str, "2") & contains(str, "1.5");
}

// Multiple Stat correctly calculated
bool t5() {
    std::cout << "testing " << __func__ << "\n";

    std::vector<float> n1 {-100, 5.32, 24};
    std::vector<std::vector<float>> n2
        {{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25}};

    util::VectorStat<float> s2{};
    s2.add(n1);
    for (auto n: n2) {
        s2.add(n);
    }
    // std::cout << s2 << "\n";
    const int c1 = s2.stats[0].min <= n1[0];
    const int c2 = s2.stats[0].max >= n1[0];
    const int c3 = s2.stats[0].num == 1 + n2.size();
    return c1 & c2 & c3;
}

int main () {
    assert( t1() ) ;
    assert( t2() ) ;
    {const int o1 = t3<std::map<int, std::string>>(); assert( o1 ) ;}
    {const int o1 = t3<std::unordered_map<int, std::string>>(); assert( o1 ) ;}
    assert( t4() );
    assert( t5() );
    return 0;
}