#include <iostream>
#include <chrono>

#include "src/expression/Expression.h"
#include "src/runge-kutta/RungeKutta.h"

int main() {
    rk::Expression<float> p;
    std::string s = "(x - y) / 2 + sin(x * pow(x, sin(y) * sin(y)))";
    p.parse(s, {"x", "y"}, utils_rk::stringToFloat);
    p.compile();
    std::cout << rk::RKSolve<float>(p, {0, 1}, 10, 0.0001)[1] << std::endl;
    std::cout << rk::RKSolveSystem<float>({std::shared_ptr<rk::Expression<float>>(&p)}, {0, 1}, 10, 0.0001)[1] << std::endl;
    return 0;
}

