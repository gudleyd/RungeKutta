#include <iostream>
#include <chrono>

#include "src/expression/Expression.h"
#include "src/runge-kutta/RungeKutta.h"
#include "test/Tests.h"

int main() {
    rk::Expression<float> p;
    std::string s = "(x - y) / 2 + sin(x * pow(x, sin(y) * sin(y)))";
    std::cout << "Running benchmark with function: " << s << "\n\n";
    p.parse(s, {"x", "y"}, utils_rk::stringToFloat);
    p.compile();
    size_t n;
    bool runBenchmarks = 0;
    std::cout << "Run Benchmarks? [1 / 0]:";
    std::cin >> runBenchmarks;
    if (runBenchmarks) {
        std::cout << "Iterations [0 - UINT32_MAX]:";
        std::cin >> n;
        std::cout << "Running benchmarks...\n";
        {
            tests_rk::OverkillTimer<95> timer1("TEST TIMER 1");
            for (size_t i = 0; i < n; ++i){
                auto result = rk::RKSolve<float>(p, {0, 1}, 10, 0.0001)[1];
                result += 1;
                timer1.reset();
            }
        }
    
        {
            tests_rk::OverkillTimer<50> timer2("TEST TIMER 2");
            for (size_t i = 0; i < n; ++i){
                auto result = rk::RKSolve<float>(p, {0, 1}, 10, 0.0001)[1];
                result += 1;
                timer2.reset();
            }
        }
        std::cout << "Finished timing\n";
    }
    {
        tests_rk::BasicTest<float> bTest1(s, {"x", "y"}, 0.0001, {{0, 0}, {0, 1}, {1, 1}, {2, 5}}, {0, -0.5, 0.841470984807897, -2.098334156388043636467});
        bTest1.run_parse_test();
    }
    {
        tests_rk::BasicTest<float> bTest2("2 * x + y", {"x", "y"}, 0.0001, {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {1, 2}}, {0, 1, 2, 3, 4});
        bTest2.run_parse_test();
    }

    return 0;
}

