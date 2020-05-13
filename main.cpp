#include <iostream>
#include <chrono>

#include "src/expression/Expression.h"
#include "src/runge-kutta/RungeKutta.h"
#include "test/Tests.h"
#include "test/tests/1.cpp"
#include "test/tests/2.cpp"
#include "test/tests/3.cpp"
#include "test/tests/4.cpp"
#include "test/tests/SolverTest1.cpp"

int main() {
    rk::Expression<float> p;
    std::string s = "(x - y) / 2 + sin(x * pow(x, sin(y) * sin(y)))";
    std::cout << "Running benchmark with function: " << s << "\n\n";
    p.parse(s, {"x", "y"}, utils_rk::stringToFloat);
    if (!p.compile()) {
        std::cout << "\nFAILED TO COMPILE FUNCTION\n";
        exit(1);
    }
    size_t n = 0;
    bool runBenchmarks = 0;
    std::cout << "Run Benchmarks? [1 / 0]:";
    std::cin >> runBenchmarks;
    if (runBenchmarks) {
        std::cout << "Iterations [0 - UINT32_MAX]:";
        std::cin >> n;
        std::cout << "\n\nRunning benchmarks...\n\n";
        {
            tests_rk::OverkillTimer<95> timer1("TEST TIMER 1");
            for (size_t i = 0; i < n; ++i){
                auto result = rk::RK4Solve<float>(p, {0, 1}, 10, 0.0001)[1];
                result += 1;
                timer1.reset();
            }
        }
    
        {
            tests_rk::OverkillTimer<50> timer2("TEST TIMER 2");
            for (size_t i = 0; i < n; ++i){
                auto result = rk::RK4Solve<float>(p, {0, 1}, 10, 0.00001)[1];
                result += 1;
                timer2.reset();
            }
        }
        std::cout << "Finished timing\n\n";
    }
    std::ofstream logOut("../test/tests/test1.log");
    if (logOut.is_open())
        mass_test_1(std::cout, logOut);
    logOut.close();
    
    logOut.open("../test/tests/test2.log");
    if (logOut.is_open())
        mass_test_2(rk::RK4Solve<long double>, 0.00001, std::cout, logOut);
    logOut.close();

    logOut.open("../test/tests/test3.log");
    if (logOut.is_open())
        mass_test_3(std::cout, logOut);
    logOut.close();

    logOut.open("../test/tests/mastersolver_1.log");
    if (logOut.is_open())
        uni_test_1(std::cout, logOut,  
        {{0, 0}, {0.5, 0.5}, {0.5, 0, 0.5}, {1, 0, 0, 1}, {0, 1.0/6, 1.0/3, 1.0/3, 1.0/6}});
    logOut.close();

    logOut.open("../test/tests/SSPRK3_test.log");
    if (logOut.is_open())
        mass_test_2(rk::SSPRK3Solve<long double>, 0.01, std::cout, logOut);
    logOut.close();
    
    logOut.open("../test/tests/ASRK4_test.log");
    if (logOut.is_open())
        ASRK_test_1(rk::ASRKFehlbergSolve<long double>, 0.001, std::cout, logOut);
    logOut.close();

    return 0;
}

