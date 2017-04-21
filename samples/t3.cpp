#include "t3.h"

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
    print(42, 21, 7, c, 3, c, 2, "#", '#');
    std::cout << c << 42 << "\n";
    std::cout << 42 << "\n";
}