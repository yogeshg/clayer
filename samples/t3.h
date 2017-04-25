#include <iostream>

template<typename T>
concept bool Streamable = requires(T o, std::ostream& s) {
    {s << o} -> std::ostream&;
};

void print(){
    std::cout << "\n";
}

template<Streamable T, typename... Targs>
void print(T o, Targs... tail) {
    std::cout << o <<" ";
    print(tail...);
}

template<typename T>
concept bool StringRef = requires(T o) {
    {*o} -> std::string;
};
