#include <iostream>

template<typename T>
concept bool Streamable = requires(T o, std::ostream& s) {
    {s << o} -> std::ostream&;
};

