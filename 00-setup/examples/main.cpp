#include <iostream>

#include "hello.hpp"

int main(int argc, char *argv[]) {
    IntCalculator calc;

    std::cout << "3 + 4 = " << calc.add(3, 4) << '\n';

    return 0;
}
