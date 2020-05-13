#include <iostream>
#include <cmath>
#include "../../src/expression/Expression.h"
#include "../../src/runge-kutta/RungeKutta.h"
#include "../Tests.h"

int mass_test_3(std::ostream& out, std::ostream& logFile) {
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
            tests_rk::SystemTest<long double> ssTest0({"y - 1", "0"}, {"x", "y", "z"}, 0.001,
            {{0, 2, 5}, {0.25, 2.28403, 5}, {1, 3.71828, 5}, {1.38629, 5, 5}},
            {{1, 0}, {1.28403, 0}, {2.71828, 0}, {4, 0}});
            tmpErrCount += ssTest0.run_solve_test({-0.5, 1.60653, 5}, 0.00001, logFile);
        }
        {
            /* 
                y' = 2y - z - 1
                z' = z - 1
                _________
                y = e^(2x) + e^(x) + 1
                z = e^(x) + 1
            */
            tests_rk::SystemTest<long double> ssTest1({"2 * y - z - 1", "z - 1"}, {"x", "y", "z"}, 0.001,
            {{0, 3, 2}, {0.25, 3.93275, 2.28403}, {0.5, 5.367, 2.64872}, {1, 11.10734, 3.71828}},
            {{3, 1}, {4.627097, 1.238403}, {7.08528, 1.64872}, {17.4964, 2.71828}});
            tmpErrCount += ssTest1.run_solve_test({-0.5, 1.97441, 1.60653}, 0.00001, logFile);
        }
       
        if (tmpErrCount > 0)
            out << "\nTotal errors: " << tmpErrCount << "\n";
        out << "Finished running system solve tests\n";
        errCount += tmpErrCount;
    }
    out << "\nFinished running mass test 3\n";
    return errCount;
}