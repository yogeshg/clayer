#include <tuple>
#include <iostream>
#include <array>
#include <utility>

template<int head>
void print_seq(const std::integer_sequence<int, head>) {
    std::cout << "print_seq"<< head <<"\n";
}

template<int head, int... I>
void print_seq(const std::integer_sequence<int, head, I...>) {
    std::cout << "print_seq"<< head <<"\n";
    print_seq(std::integer_sequence<int,I...>());
}

int main()
{
    std::array<int, 4> array = {1,2,3,4};
    print_seq(std::integer_sequence<int,1,3,4,2>());
    // print_seq(std::make_index_sequence<>)
    return 0;
}