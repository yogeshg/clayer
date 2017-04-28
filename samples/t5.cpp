#include <iostream>
#include <regex>

int main(){
    std::regex format_regex("\\[(.*) (.*)\\] (.*)\\[Thread (.*):(.*)\\((.*):(.*)\\)\\]: \\[(.*)\\]");
    std::string line("[2017-04-27 11:29:05] DEBUG[Thread 0x7fff7317a310:src/runner.cpp(main:23)]: [Begin logging]");
    std::smatch m;
    std::regex_match(line, m, format_regex);

    for(auto it : m) {
        std::cout << it << "\n";
    }
    std::cout << "\n";

}