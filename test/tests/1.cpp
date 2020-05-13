#include <iostream>
#include <cmath>
#include "../../src/expression/Expression.h"
#include "../../src/runge-kutta/RungeKutta.h"
#include "../Tests.h"


static float rand_float() {
    return static_cast<float>(rand() % ((1ull << 23u) + 1));
}


int mass_test_1(std::ostream& out, std::ostream& logFile) {
    out << "Running mass test 1\n";
    size_t errCount = 0;
    {   /*  PARSE TESTS */

        size_t tmpErrCount = 0;
        const double pi = 3.141592653589793;
        const double e = 2.718281828459046;
        out << "\nRunning parse tests...\n";
        {
            tests_rk::BasicTest<float> bTest0("5 + x", {"x"}, 0.0001, {{0}, {1}, {-1}, {0.5}, {-0.5}, {5}, {-5}}, 
            {5, 6, 4, 5.5, 4.5, 10, 0});
            tmpErrCount += bTest0.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest1("5", {}, 0.000001, {{0}, {1}, {-1}, {0.5}, {-0.5}, {5}, {-5}}, 
            {5, 5, 5, 5, 5, 5, 5});
            tmpErrCount += bTest1.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest2("a + 0", {"a"}, 0.000001, {{0}, {1}, {-1}, {0.5}, {-0.5}, {5}, {-5}}, 
            {0, 1, -1, 0.5, -0.5, 5, -5});
            tmpErrCount += bTest2.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest3("2 * x + y", {"x", "y"}, 0.0001, {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {1, 2}}, 
            {0, 1, 2, 3, 4});
            tmpErrCount += bTest3.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest4("x + y + z", {"x", "y", "z"}, 0.0001, 
            {{-1, 0, 1}, {-2, 1, 1}, {1, -2, 1}, {0, 0, 0}, {0.33333334, 0.33333334, 0.33333334}, 
            {0, 0, 1}, {0, 1, 0}, {0, 1, 1}, {1, 0, 0}, {1, 0, 1}, {1, 1, 0}, {1, 1, 1}}, 
            {0, 0, 0, 0, 1, 1, 1, 2, 1, 2, 2, 3});
            tmpErrCount += bTest4.run_parse_test(logFile);   
        }
        {
            tests_rk::BasicTest<float> bTest5("-0", {}, 0.000001, {{0}, {1}, {-1}, {0.5}, {-0.5}, {5}, {-5}}, 
            {0, 0, 0, 0, 0, 0, 0});
            tmpErrCount += bTest5.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest6("sin(x)", {"x"}, 0.0001, {{0}, {pi / 2.0}, {pi}, {1.5 * pi}, {2 * pi}, {pi / 4}}, 
            {0, 1, 0, -1, 0, sqrt(2.0)/2.0});
            tmpErrCount += bTest6.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest7("sin(3927 * x / 1250)", {"x"}, 0.0001, {{0}, {0.5}, {1}, {1.5}, {2}, {0.25}}, 
            {0, 1, 0, -1, 0, sqrt(2.0)/2.0});
            tmpErrCount += bTest7.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest8("0 + sin(3927 * x/1250)", {"x"}, 0.0001, {{0}, {0.5}, {1}, {1.5}, {2}, {0.25}}, 
            {0, 1, 0, -1, 0, sqrt(2.0)/2.0});
            tmpErrCount += bTest8.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest7("-sin(3927 * x/1250)", {"x"}, 0.0001, {{0}, {0.5}, {1}, {1.5}, {2}, {0.25}}, 
            {0, -1, 0, 1, 0, -sqrt(2.0)/2.0});
            tmpErrCount += bTest7.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest8("1 - sin(3927 * x/1250)", {"x"}, 0.0001, {{0}, {0.5}, {1}, {1.5}, {2}, {0.25}}, 
            {1, 0, 1, 2, 1, 1 - sqrt(2.0)/2.0});
            tmpErrCount += bTest8.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest9("y - sin(3927 * x/1250)", {"x", "y"}, 0.0001, 
            {{0, 1}, {0.5, 1}, {1, 1}, {1.5, 1}, {2, 1}, {0.25, 1}, {0, 0}, {0.5, 0}, {1, 0}, {1.5, 0}, {2, 0}, {0.25, 0}}, 
            {1, 0, 1, 2, 1, 1 - sqrt(2.0)/2.0, 0, -1, 0, 1, 0, -sqrt(2.0)/2.0});
            tmpErrCount += bTest9.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest10("sin(" + std::to_string(pi) + " * x)", {"x"}, 0.0001, {{0}, {0.5}, {1}, {1.5}, {2}, {0.25}}, 
            {0, 1, 0, -1, 0, sqrt(2.0)/2.0});
            tmpErrCount += bTest10.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest11("0 + sin(" + std::to_string(pi) + " * x)", {"x"}, 0.0001, {{0}, {0.5}, {1}, {1.5}, {2}, {0.25}}, 
            {0, 1, 0, -1, 0, sqrt(2.0)/2.0});
            tmpErrCount += bTest11.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest12("-sin(" + std::to_string(pi) + " * x)", {"x"}, 0.0001, {{0}, {0.5}, {1}, {1.5}, {2}, {0.25}}, 
            {0, -1, 0, 1, 0, -sqrt(2.0)/2.0});
            tmpErrCount += bTest12.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest13("1 - sin(" + std::to_string(pi) + " * x)", {"x"}, 0.0001, {{0}, {0.5}, {1}, {1.5}, {2}, {0.25}}, 
            {1, 0, 1, 2, 1, 1 - sqrt(2.0)/2.0});
            tmpErrCount += bTest13.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest14("y - sin(" + std::to_string(pi) + " * x)", {"x", "y"}, 0.0001, 
            {{0, 1}, {0.5, 1}, {1, 1}, {1.5, 1}, {2, 1}, {0.25, 1}, {0, 0}, {0.5, 0}, {1, 0}, {1.5, 0}, {2, 0}, {0.25, 0}}, 
            {1, 0, 1, 2, 1, 1 - sqrt(2.0)/2.0, 0, -1, 0, 1, 0, -sqrt(2.0)/2.0});
            tmpErrCount += bTest14.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest15("0.5", {}, 0.000001, {{0}, {1}, {-1}, {2}, {-2}, {3}, {-3}, {0.5}, {-0.5}}, 
            {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5});
            tmpErrCount += bTest15.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest16("0.5 * x", {"x"}, 0.0001, {{0}, {1}, {-1}, {2}, {-2}, {3}, {-3}, {0.5}, {-0.5}}, 
            {0, 0.5, -0.5, 1, -1, 1.5, -1.5, 0.25, -0.25});
            tmpErrCount += bTest16.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest17("0.000 * x * (y + z) * (z * (-w))", {"x", "y", "z", "v", "w"}, 0.000001, 
            {{0, 0, 0, 0, 0}, {1, rand_float(), rand_float(), rand_float(), rand_float()}, 
            {-1, rand_float(), rand_float(), rand_float(), rand_float()}, {2, 0, 0, 1, 2}, 
            {-2, rand_float(), rand_float(), rand_float(), rand_float()}, {3, 5, 2, 1, 9}, 
            {-3, rand_float(), rand_float(), rand_float(), rand_float()}, 
            {0.5, rand_float(), rand_float(), rand_float(), rand_float()}, 
            {rand_float() , rand_float(), rand_float(), rand_float(), rand_float()}}, 
            {0, 0, 0, 0, 0, 0, 0, 0, 0});
            tmpErrCount += bTest17.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest18("0.5 * (x + (y + z) - y - (x + z)) * (z * (-w))", {"x", "y", "z", "v", "w"}, 0.000001, 
            {{0, 0, 0, 0, 0}, {1, rand_float(), rand_float(), rand_float(), rand_float()}, 
            {-1, rand_float(), rand_float(), rand_float(), rand_float()}, {2, 0, 0, 1, 2}, 
            {-2, rand_float(), rand_float(), rand_float(), rand_float()}, {3, 5, 2, 1, 9}, 
            {-3, rand_float(), rand_float(), rand_float(), rand_float()}, 
            {0.5, rand_float(), rand_float(), rand_float(), rand_float()}, 
            {rand_float() , rand_float(), rand_float(), rand_float(), rand_float()}}, 
            {0, 0, 0, 0, 0, 0, 0, 0, 0});
            tmpErrCount += bTest18.run_parse_test(logFile);
        }
        {
            tests_rk::BasicTest<float> bTest19("pow(" + std::to_string(e) +", x)", {"x"}, 0.001, 
            {{0}, {1}, {-1}, {0.5}, {-0.5}, {1.5}, {2}}, 
            {1, e, 1.0/e, 1.64872, 0.606531, 4.48169, e * e});
            tmpErrCount += bTest19.run_parse_test(logFile);
        }
        if (tmpErrCount > 0)
            out << "\nTotal errors: " << tmpErrCount << "\n";
        out << "Finished running parse tests\n";
        errCount += tmpErrCount;
    }
    out << "\nFinished running mass test 1\n";
    return errCount;
}