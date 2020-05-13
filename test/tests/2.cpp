#include <iostream>
#include <cmath>
#include "../../src/expression/Expression.h"
#include "../../src/runge-kutta/RungeKuttaMethods.h"
#include "../Tests.h"

int mass_test_2(std::vector<long double> (*solver)(const rk::Expression<long double>&, std::vector<long double>, long double, long double),
                long double delta,
                std::ostream& out, 
                std::ostream& logFile
                ) {
    out << "Running mass test 2\n";
    size_t errCount = 0;
    {   /*  SOLVE TESTS */

        size_t tmpErrCount = 0;
        const long double pi = 3.141592653589793239;
        const std::string eStr = "2.7182818284590452354";
        out << "\nRunning solve tests...\n";
        std::vector<std::vector<long double>> bT = {{0, 0}, {0.5, 0.5}, {0.5, 0, 0.5}, {1, 0, 0, 1}, {0, 1.0/6, 1.0/3, 1.0/3, 1.0/6}};
        /*
        {
            // y = e^(2x) [floats]
            tests_rk::BasicTest<float> sTest0("2 * y", {"x", "y"}, 0.005, {{0, 1}, {1, 7.38906}, {2, 54.59815}, {0.5, 2.71828}, {1.5, 20.08554}}, 
            {2, 14.77812, 0.27067, 109.1963, 0.0366312, 5.43656, 0.735758});
            tmpErrCount += sTest0.run_solve_test({0, 1}, 0.00001, logFile, solver);
        }
        */
        {
            // y = e^(2x) [long doubles]
            tests_rk::BasicTest<long double> sTest1("2 * y", {"x", "y"}, delta, {{0.0, 1}, {1, 7.38906}, {2, 54.59815}, {0.5, 2.71828}, {1.5, 20.08554}}, 
            {2, 14.77812, 0.27067, 109.1963, 0.0366312, 5.43656, 0.735758});
            tmpErrCount += sTest1.run_solve_test({0, 1}, 0.00001, logFile, solver);
        }
        {
            // y = sin(x) * e^(2x) + 1
            tests_rk::BasicTest<long double> sTest2("2 * y + ( 1 - (2 * sin(0.5 * x) * sin(0.5 * x)) ) * y * (1.0 / sin(x))", {"x", "y"}, delta, 
            {{0, 1}, {0.25, 1.4079}, {0.5, 2.30321}, {pi/4, 4.40152}, {1, 7.21768}, {pi/3, 8.03258}}, 
            {1, 2.41327, 4.99194, 10.20456353, 16.4276766731772, 18.12542973363});
            tmpErrCount += sTest2.run_solve_test({0, 1}, 0.00001, logFile, solver);
        }
        {
            // y =  e^(sin(x)^2) - 1

            // Note: Finding a solution to this one is a bit awkward
            // this basically looks like a sine wave https://www.desmos.com/calculator/0xmduvdons
            tests_rk::BasicTest<long double> sTest3("sin(2 * x) * (y + 1)", {"x", "y"}, delta, 
            {{0.25, 0.0631208}, {pi/6, 0.28402541668774148}, {pi/4, 0.648721270700128147}, {pi/3, 1.117000016612675}, {pi/2, 1.71828182845904523536}}, 
            {0.509687, 1.1119986299564834, 1.648721270700128147, 1.833375794198654903, 0});
            tmpErrCount += sTest3.run_solve_test({0.1, 0.0100165}, 0.00001, logFile, solver);
        }
        
        {
            // y =  e^e^e^x

            // Note: this one is kinda hard to solve so lower precision
            tests_rk::BasicTest<long double> sTest4("y * pow(" + eStr + ", pow(" + eStr + ", x)) * pow(" + eStr + ", x)", 
            {"x", "y"}, delta, 
            {{-0.5, 6.25923128218}, {-0.25, 8.83620755526}, {0, 15.1542622415}, {0.1, 20.4859759685}, {0.2, 29.7236335842}, {0.5, 181.331303609}}, 
            {6.96284419035, 14.994126505, 41.1935556747, 68.3684506915, 123.142870627, 1554.71423417});
            tmpErrCount += sTest4.run_solve_test({-1, 4.24044349228}, 0.00001, logFile, solver);
        }
        {
            // y =  e^( -e^(sin(x) - cos(x)) )

            tests_rk::BasicTest<long double> sTest5("(y - 1) * pow(" + eStr + ", sin(x) - (1 - 2 * sin(x/2) * sin(x/2)) ) * ( - sin(x) - 1 + 2 * sin(x/2) * sin(x/2) )", 
            {"x", "y"}, delta, 
            {{0, 1.6922006275}, {0.25, 1.61507039029}, {0.5, 1.51091268748}, {1, 1.2588679271}, {1.5, 1.0799538999743}, {2.35, 1.01635635493}}, 
            {-0.254646380044, -0.3636, -0.465599, -0.483404774008, -0.21577, -0.000589356 });
            tmpErrCount += sTest5.run_solve_test({-1, 1.77791903645}, 0.00001, logFile, solver);
        }
        {
            // y =  sqrt(x) +1/x

            tests_rk::BasicTest<long double> sTest6("( 1 / (2 * (y - (1/x))) ) - (1/(pow(x, 2)))", 
            {"x", "y"}, delta, 
            {{0.1, 10.316227766}, {0.25, 4.5}, {0.5, 2.70710678119}, {0.75, 2.19935873712}, {1, 2}, {1.587401051968199472, 1.88988157484} ,{2.6180339888, 2}}, 
            {-98.4188611699, -15, -3.29289321881, -1.20042750859, -0.5, 0, 0.163118960627});
            tmpErrCount += sTest6.run_solve_test({0.1, 10.316227766}, 0.00001, logFile, solver);
        }
        {
            // y =  x + 5

            tests_rk::BasicTest<long double> sTest7("1", 
            {"x", "y"}, delta, 
            {{-1, 4}, {-0.5, 4.5}, {0.1, 5.1}, {0.25, 5.25}, {0.5, 5.5}, {0.75, 5.75}, {1, 6}, {5, 10} }, 
            {1, 1, 1, 1, 1, 1, 1, 1});
            tmpErrCount += sTest7.run_solve_test({-5, 0}, 0.00001, logFile, solver);
        }
        {
            // y =  x^2

            tests_rk::BasicTest<long double> sTest8("2 * x", 
            {"x", "y"}, delta, 
            {{-1, 1}, {-0.5, 0.25}, {0, 0}, {0.5, 0.25}, {1, 1}, {2, 4}, {5, 25} }, 
            {-2, -1, 0, 1, 2, 4, 10});
            tmpErrCount += sTest8.run_solve_test({-5, 25}, 0.00001, logFile, solver);
        }
        {
            // y = 100

            tests_rk::BasicTest<long double> sTest8("0 * x + 0 * y", 
            {"x", "y"}, delta, 
            {{-1, 100}, {-0.5, 100}, {0, 100}, {0.5, 100}, {1, 100}, {2, 100}, {5, 100} }, 
            {0, 0, 0, 0, 0, 0, 0});
            tmpErrCount += sTest8.run_solve_test({-100, 100}, 0.00001, logFile, solver);
        }
        if (tmpErrCount > 0)
            out << "\nTotal errors: " << tmpErrCount << "\n";
        out << "Finished running solve tests\n";
        errCount += tmpErrCount;
    }
    out << "\nFinished running mass test 2\n";
    return errCount;
}