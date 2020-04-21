#include <iostream>

#include "src/expression/Expression.h"
#include "src/runge-kutta/RungeKutta.h"

int main() {
    rk::Expression<long double> p;
    std::string s = "sin(x) * sin(y) + x * y -213 + (-(y * x * x) * 57) + pow(3, 4)";
    p.parse(s, {"x", "y"}, utils_rk::stringToLongDouble);
    std::cout << p.evaluate({2, 3}) << std::endl;
    std::cout << (p.compile() ? "Compiled" : "Not compiled") << std::endl;
    std::cout << p.evaluate({2, 3}) << std::endl;
    return 0;
}
