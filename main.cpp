#include <iostream>

#include "src/expression/Expression.h"
#include "src/runge-kutta/RungeKutta.h"

int main() {
    rk::Expression<float> p;
    std::string s = "(x - y) / 2";
    p.parse(s, {"x", "y"}, utils_rk::stringToFloat);
    std::cout << p.evaluate({2, 3}) << std::endl;
    std::cout << rk::RK_solve_one<float>(p, {0, 1}, 2, 0.2) << std::endl;
    return 0;
}
