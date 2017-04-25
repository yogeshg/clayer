#include <tuple>
#include <iostream>
#include <array>
#include <utility>
#include <string>
#include "t3.h"

enum {
    DATE,
    TIME,
    FUNC
};

template<int prop>
Streamable get_prop();

template<>
Streamable get_prop<DATE>() {
    return "date";
}
template<>
Streamable get_prop<TIME>() {
    return "time";
}
template<>
Streamable get_prop<FUNC>() {
    return "func";
}

template<typename T = void>
void print_props();

template<>
void print_props<>(){
    std::cout <<"\n";
}

template<int head, int... I>
void print_props() {
    std::cout << get_prop<head>() <<" "<< head <<"\t";
    print_props<I...>();
}


// Adding custom types to be printed

template<>
Streamable get_prop<1500>() {
    return "mytype";
}

int main()
{
    print_props<DATE, FUNC, TIME>();
    print_props<TIME, FUNC, DATE>();
    print_props<FUNC, DATE, TIME>();
    print_props<FUNC, DATE, 1500>();
    return 0;
}