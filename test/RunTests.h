//
// Created by lebedev-ivan on 13.05.2020.
//

#pragma once

#include <ostream>
#include <chrono>

#include "../src/expression/Expression.h"
#include "../src/runge-kutta/RungeKuttaMethods.h"
#include "Tests.h"
#include "tests/1.cpp"
#include "tests/2.cpp"
#include "tests/3.cpp"
#include "tests/4.cpp"
#include "tests/SolverTest1.cpp"

namespace tests_rk {

    void run_timer(std::vector<double> (*solver)(const rk::Expression<double>&, std::vector<double>, double, double),
                   size_t n, rk::Expression<double> expr, std::vector<double> init, double at, double eps, std::string timerName) {
        {
            tests_rk::OverkillTimer<75> timer1(timerName + " Timer 1");
            for (size_t i = 0; i < n; ++i){
                auto result = solver(expr, init, at, eps)[1];
                result += 1;
                timer1.reset();
            }
        }
        {
            tests_rk::OverkillTimer<50> timer2(timerName + " Timer 2");
            for (size_t i = 0; i < n; ++i){
                auto result = solver(expr, init, at, eps)[1];
                result += 1;
                timer2.reset();
            }
        }
    }

    
    void run_basic_test(std::vector<long double> (*solver)(const rk::Expression<long double>&, std::vector<long double>, long double, long double),
                        std::string name, std::ostream& out, long double delta, long double precision, int bitMask) {
        std::string f_name(name);
        f_name.erase(std::remove_if(f_name.begin(), f_name.end(), [](unsigned char x){return std::isspace(x);}), f_name.end());
        for (auto it = f_name.begin(); it != f_name.end(); ++it) {
            *it = std::tolower(*it);
        }
        std::ofstream logOut("../test/tests/logs/" + name + ".log");
        if (logOut.is_open())
            mass_test_2(solver, delta, precision, out, logOut, name, bitMask);
        logOut.close();
    }

    void runTests(std::ostream& out, bool runBenchmarks = false) {
        rk::Expression<double> p;
        std::string s = "cos(x) * y";
        out << "Running benchmark with function: " << s << "\n\n";
        p.parse(s, {"x", "y"}, utils_rk::stringToDouble);
        if (!p.compile()) {
            out << "\nFAILED TO COMPILE FUNCTION\n";
            exit(1);
        }
        size_t n = 0;
        if (runBenchmarks) {
            std::cout << "Iterations [0 - UINT32_MAX]:";
            std::cin >> n;
            out << "\n\nRunning benchmarks...\n\n";
            out << "\nRK second order\n";
            //run_timer(rk::RK2GenericSolve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "RK2Generic");
            run_timer(rk::RK2HeunSolve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "RK2Heun");
            run_timer(rk::RK2MidpointSolve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "RK2Midpoint");
            run_timer(rk::RK2RalstonSolve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "RK2Ralston");
            out << "\nRK third order\n";
            //run_timer(rk::RK3GenericSolve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "RK3Generic");
            run_timer(rk::RK3HeunSolve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "RK3Heun");
            run_timer(rk::RK3RalstonSolve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "RK3Ralston");
            run_timer(rk::SSPRK3Solve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "SSPRK3");
            out << "\nRK fourth order\n";
            run_timer(rk::RK4Solve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "RK4Solve");
            run_timer(rk::RK4ClassicSolve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "RK4Classic");
            run_timer(rk::RK4RalstonSolve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "RK4Ralston");
            run_timer(rk::SSPRK4Solve<double>, n, p, {-0.5, 0.619138961097731}, 0, 0.0001, "SSPRK4");
            out << "Finished timing\n\n";
        }
        std::ofstream logOut("../test/tests/logs/test1.log");
        //if (logOut.is_open())
        //    mass_test_1(out, logOut);
        logOut.close();

        //run_basic_test<rk::RK2GenericSolve<long double>>("RK2Generic", out, 0.01L, 0.00001L, 0x1F1);
        // run_basic_test(rk::RK2HeunSolve<long double>, "RK2Heun", out, 0.01L, 0.00001L, 0x1F1);
        // run_basic_test(rk::RK2MidpointSolve<long double>, "RK2Midpoint", out, 0.01L, 0.00001L, 0x1F1);
        // run_basic_test(rk::RK2RalstonSolve<long double>, "RK2Ralston", out, 0.01L, 0.00001L, 0x1F1);

        // //run_basic_test<rk::RK3GenericSolve<long double>>("RK3Generic", out, 0.01L, 0.00001L, 0x3FD);
        // run_basic_test(rk::RK3HeunSolve<long double>, "RK3Heun", out, 0.01L, 0.00001, 0x1FD);
        // run_basic_test(rk::RK3RalstonSolve<long double>, "RK3Ralston", out, 0.01L, 0.00001L, 0x1FD);
        // run_basic_test(rk::SSPRK3Solve<long double>, "SSPRK3", out, 0.01L, 0.00001L, 0x1F1);

        // run_basic_test(rk::RK4Solve<long double>, "RK4", out, 0.001L, 0.00001L, 0x1FF);
        // run_basic_test(rk::RK4ClassicSolve<long double>, "RK4Classic", out, 0.001L, 0.00001L, 0x1FF);
        // run_basic_test(rk::RK4RalstonSolve<long double>, "RK4Ralston", out, 0.001L, 0.00001L, 0x1FF);
        // run_basic_test(rk::SSPRK4Solve<long double>,"SSPRK4", out, 0.001L, 0.00001L, 0x1F1);

        // run_basic_test(rk::SSPRK5Solve<long double>,"SSPRK5", out, 0.001L, 0.00001L, 0x1F1);
        
        // logOut.open("../test/tests/logs/RK4System.log");
        // if (logOut.is_open())
        //     mass_test_3(out, logOut, 0.001 , 0.0001);
        // logOut.close();

        // logOut.open("../test/tests/logs/MasterSolver.log");
        // if (logOut.is_open())
        //     uni_test_1(out, logOut,
        //                {{0, 0}, {0.5, 0.5}, {0.5, 0, 0.5}, {1, 0, 0, 1}, {0, 1.0/6, 1.0/3, 1.0/3, 1.0/6}});
        // logOut.close();
        
        // logOut.open("../test/tests/logs/ASRKFehlberg.log");
        // if (logOut.is_open())
        //     ASRK_test_1(rk::ASRKFehlbergSolve<long double>, 0.01, out, logOut);
        // logOut.close();
        logOut.open("../test/tests/logs/ASRKCashCarp.log");
        if (logOut.is_open())
             ASRK_test_1(rk::ASRKCashCarpSolve<long double>, 0.01, out, logOut);
         logOut.close();
        // logOut.open("../test/tests/logs/ASRKDormandPrince.log");
        // if (logOut.is_open())
        //     ASRK_test_1(rk::ASRKDormandPrinceSolve<long double>, 0.01, out, logOut);
        // logOut.close();
        
    }
}