#include <tuple>
#include <iostream>
#include <array>
#include <utility>

enum {
    DATE,
    TIME,
    FUNC
};

template<int prop>
void print_prop();

template<>
void print_prop<DATE>() {
    std::cout << "date";
}
template<>
void print_prop<TIME>() {
    std::cout << "time";
}
template<>
void print_prop<FUNC>() {
    std::cout << "func";
}

template<typename T = void>
void print_seq();

template<>
void print_seq<>(){
    std::cout <<"\n";
}

template<int head, int... I>
void print_seq() {
    print_prop<head>();
    std::cout << head <<"\t";
    print_seq<I...>();
}

int main()
{
    print_seq<DATE, FUNC, TIME>();
    print_seq<TIME, FUNC, TIME>();
    print_seq<FUNC, FUNC, TIME>();
    return 0;
}