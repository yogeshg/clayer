#include "t3.h"

void print(Streamable o) {
    std::cout << o << std::endl;
}

void printall(){
    
}

template<Streamable T, typename... Targs>
void printall(T o, Targs... tail) {
    std::cout << o <<" ";
    printall(tail...);
}

class C {

    public:
        C(int a, int b) : x(a), y(b) {}

    // private:
        int x;
        int y;
};

std::ostream& operator<<(std::ostream& s, C c) {
    return s <<"(" << c.x << " " << c.y << ")";
}

int main() {
    C c(4,2);
    printall(42, 21, 7);
    print(42);
    print(c);
    std::cout << c << 42 << "\n";
    std::cout << 42 << "\n";
}