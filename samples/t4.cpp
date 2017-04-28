#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include "t3.h"

enum Poperties {
    DATE,
    LINE,
    FUNC
};


std::string date;
std::string func;
int line;

template<int N>
void read_prop(std::istringstream s);

template<>
void read_prop<DATE>(std::istringstream is){
    is >> date;
}

template<>
void read_prop<FUNC>(std::istringstream is){
    is >> func;
}

template<>
void read_prop<LINE>(std::istringstream is){
    is >> line;
}

// template<StringRef P,typename T = void>
// void read_props(P);

template<StringRef P>
void read_props(P){
}

template<StringRef P,int head, int... I>
void read_props(P p) {
    read_prop<head>(std::istringstream(*p));
    p++;
    read_props<P,I...>(p);
}

