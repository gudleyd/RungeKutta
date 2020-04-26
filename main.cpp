#include <iostream>
#include <chrono>

#include "src/expression/Expression.h"
#include "src/runge-kutta/RungeKutta.h"
#include "test/Tests.h"

int main() {
    rk::Expression<float> p;
    std::string s = "(x - y) / 2 + sin(x * pow(x, sin(y) * sin(y)))";
    p.parse(s, {"x", "y"}, utils_rk::stringToFloat);
    p.compile();
    {
        tests_rk::OverkillTimer<95> timer1("TEST TIMER 1");
        for (size_t i = 0; i < 300; ++i){
            auto result = rk::RKSolve<float>(p, {0, 1}, 10, 0.0001)[1];
            result += 1;
            //std::cout << rk::RKSolveSystem<float>({std::shared_ptr<rk::Expression<float>>(&p)}, {0, 1}, 10, 0.0001)[1] << std::endl;
            timer1.reset();
        }
    }
    {
        tests_rk::OverkillTimer<50> timer2("TEST TIMER 2");
        for (size_t i = 0; i < 300; ++i){
            auto result = rk::RKSolve<float>(p, {0, 1}, 10, 0.0001)[1];
            result += 1;
            //std::cout << rk::RKSolveSystem<float>({std::shared_ptr<rk::Expression<float>>(&p)}, {0, 1}, 10, 0.0001)[1] << std::endl;
            timer2.reset();
        }
    }
    std::cout << "Finished testing\n";
    return 0;
}

