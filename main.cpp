#include <iostream>

#include "src/expression/Expression.h"

int main() {
    Expression<long double> p;
    p.parse("-x * x * x + y * y", {"x", "y"}, utils_rk::stringToLongDouble);
    std::cout << p.evaluate({2, 3}) << std::endl;
    return 0;
}
