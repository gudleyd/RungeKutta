#include <iostream>
#include <cmath>
#include "../../src/expression/Expression.h"
#include "../../src/runge-kutta/RungeKuttaMethods.h"
#include "../Tests.h"

int mass_test_3(std::ostream& out, 
                std::ostream& logFile,
                double precision,
                double delta,
                std::vector<long double> (*solver)(const std::vector<std::shared_ptr<rk::Expression<long double>>>&, std::vector<long double>, long double, long double) = rk::RK4SystemSolve) {
    out << "Running mass test 3\n";
    size_t errCount = 0;
    {   /*  SYSTEM SOLVE TESTS */

        size_t tmpErrCount = 0;
        const std::string eStr = "2.7182818284590452354";
        out << "\nRunning system solve tests...\n";
        {
            /* 
                y' = y - 1
                z' = 0
                _________
                y = e^(x) + 1
                z = 5
            */
            tests_rk::SystemTest<long double> ssTest0({"y - 1", "0"}, {"x", "y", "z"}, precision,
            {{0, 2, 5}, {0.25, 2.28403, 5}, {1, 3.71828, 5}, {1.38629, 5, 5}},
            {{1, 0}, {1.28403, 0}, {2.71828, 0}, {4, 0}});
            tmpErrCount += ssTest0.run_solve_test({-0.5, 1.60653, 5}, delta, logFile, solver);
        }
        {
            /* 
                y' = 2y - z - 1
                z' = z - 1
                _________
                y = e^(2x) + e^(x) + 1
                z = e^(x) + 1
            */
            tests_rk::SystemTest<long double> ssTest1({"2 * y - z - 1", "z - 1"}, 
            {"x", "y", "z"}, 
            precision,
            {{0, 3, 2}, {0.25, 3.93275, 2.28403}, {0.5, 5.367, 2.64872}, {1, 11.10734, 3.71828}},
            {{3, 1}, {4.627097, 1.238403}, {7.08528, 1.64872}, {17.4964, 2.71828}});
            tmpErrCount += ssTest1.run_solve_test({-0.5, 1.97441, 1.60653}, delta, logFile, solver);
        }
        {
            /*
                y' = y(z - cos(x + cos(x))(sin(x) - 1))
                z' = (cos(x) + 1)^2 cos(x + sin(x)) - z sin(x)/(cos(x) + 1)
                _________
                y = e^(sin(x + cos(x)) - cos(x + sin(x)))
                z = sin(x + sin(x))(cos(x) + 1)
            */
            tests_rk::SystemTest<long double> ssTest2(
                {"y * (z - cos(x + cos(x)) * (sin(x) - 1))", "(cos(x) + 1) * (cos(x) + 1) * cos(x + sin(x)) - z * sin(x)/(cos(x) + 1)"}, 
                {"x", "y", "z"}, 
                precision,
                {{-1, 0.8383701135, -1.4842210149}, {-0.75, 0.854688, -1.71495}, {-0.5, 0.827931, -1.55873}, {-0.25, 0.802312, -0.939458}},
                {{0.139237, -1.4452}, {-0.0287108, -0.259097}, {-0.151937, 1.56735}, {-0.000608654, 3.28882}}
                );
                tmpErrCount += ssTest2.run_solve_test({-1.1, 0.823506, -1.32702}, delta, logFile, solver);
        }
       
        if (tmpErrCount > 0)
            out << "\nTotal errors: " << tmpErrCount << "\n";
        out << "Finished running system solve tests\n";
        errCount += tmpErrCount;
    }
    out << "\nFinished running mass test 3\n";
    return errCount;
}