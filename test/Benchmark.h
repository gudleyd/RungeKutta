//
// Created by lebedev-ivan on 18.05.2020.
//

#pragma once

#include "../src/expression/Expression.h"
#include "../src/runge-kutta/RungeKuttaMethods.h"
#include "Tests.h"

namespace tests_rk {

    void runBenchmark(std::vector<double> (*solver)(const rk::Expression<double>&, std::vector<double>, double, double),
                      size_t n, rk::Expression<double> expr, std::vector<double> init, double at, double eps, std::string timerName) {
        {
            tests_rk::OverkillTimer<50, millisec> timer2(timerName + " 1.000.000 Points");
            for (size_t i = 0; i < n; ++i) {
                std::vector<double> result = init;
                double at2 = at;
                for (int j = 0; j < 1000000; ++j) {
                    result = solver(expr, result, at, eps);
                    at = result[0] + 0.001;
                }
                at = at2;
                timer2.reset();
            }
        }
    }

    void Benchmark() {
        int n = 6;
        rk::Expression<double> p;
        p.parse("y * ((sin(x)) / x + (cos(x)) / (x * x))", {"x", "y"});
        p.compile();

        runBenchmark(rk::RK2HeunSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "RK2Heun");
        runBenchmark(rk::RK2MidpointSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "RK2Midpoint");
        runBenchmark(rk::RK2RalstonSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "RK2Ralston");

        runBenchmark(rk::RK3Solve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "RK3Solve");
        runBenchmark(rk::RK3HeunSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "RK3Heun");
        runBenchmark(rk::RK3RalstonSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "RK3Ralston");
        runBenchmark(rk::SSPRK3Solve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "SSPRK3");

        runBenchmark(rk::RK4Solve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "RK4Solve");
        runBenchmark(rk::RK4ClassicSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "RK4Classic");
        runBenchmark(rk::RK4RalstonSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "RK4Ralston");
        runBenchmark(rk::SSPRK4Solve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "SSPRK4");

        runBenchmark(rk::SSPRK5Solve<double>, n, p, {5, 0.944846841517}, 5.001, 0.0001, "SSPRK5");

        runBenchmark(rk::ASRKDormandPrinceSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.01, "ASRKDormandPrince");
        runBenchmark(rk::ASRKBogackiShampineSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.01, "ASRKBogackiShampine");
        runBenchmark(rk::ASRKCashCarpSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.01, "ASRKCashCarp");
        runBenchmark(rk::ASRKFehlbergSolve<double>, n, p, {5, 0.944846841517}, 5.001, 0.01, "ASRKFehlberg");
    }
}