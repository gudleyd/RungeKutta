//
// Created by lebedev-ivan on 13.05.2020.
//

#pragma once

#include <ostream>
#include <chrono>

#include "../src/expression/Expression.h"
#include "../src/runge-kutta/RungeKutta.h"
#include "Tests.h"
#include "tests/1.cpp"
#include "tests/2.cpp"
#include "tests/3.cpp"
#include "tests/4.cpp"
#include "tests/SolverTest1.cpp"

namespace tests_rk {

    void runTests(std::ostream& out, bool runBenchmarks = false) {
        rk::Expression<float> p;
        std::string s = "(x - y) / 2 + sin(x * pow(x, sin(y) * sin(y)))";
        out << "Running benchmark with function: " << s << "\n\n";
        p.parse(s, {"x", "y"}, utils_rk::stringToFloat);
        if (!p.compile()) {
            out << "\nFAILED TO COMPILE FUNCTION\n";
            exit(1);
        }
        size_t n = 0;
        out << "Run Benchmarks? [1 / 0]:";
        if (runBenchmarks) {
            out << "Iterations [0 - UINT32_MAX]:";
            out << "\n\nRunning benchmarks...\n\n";
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
            out << "Finished timing\n\n";
        }
        std::ofstream logOut("../test/tests/logs/test1.log");
        if (logOut.is_open())
            mass_test_1(out, logOut);
        logOut.close();

        logOut.open("../test/tests/logs/test2.log");
        if (logOut.is_open())
            mass_test_2(rk::RK4Solve<long double>, 0.00001, out, logOut);
        logOut.close();

        logOut.open("../test/tests/logs/test3.log");
        if (logOut.is_open())
            mass_test_3(out, logOut);
        logOut.close();

        logOut.open("../test/tests/logs/mastersolver_1.log");
        if (logOut.is_open())
            uni_test_1(out, logOut,
                       {{0, 0}, {0.5, 0.5}, {0.5, 0, 0.5}, {1, 0, 0, 1}, {0, 1.0/6, 1.0/3, 1.0/3, 1.0/6}});
        logOut.close();

        logOut.open("../test/tests/logs/SSPRK3_test.log");
        if (logOut.is_open())
            mass_test_2(rk::SSPRK3Solve<long double>, 0.01, out, logOut);
        logOut.close();

        logOut.open("../test/tests/logs/ASRK4_test.log");
        if (logOut.is_open())
            ASRK_test_1(rk::ASRKFehlbergSolve<long double>, 0.001, out, logOut);
        logOut.close();
    }
}