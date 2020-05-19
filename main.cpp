#include <iostream>

#include "test/RunTests.h"
#include "test/Benchmark.h"

void runTestsInStd() {
    tests_rk::runTests(std::cout, true);
    tests_rk::Benchmark();
}

int main() {
    runTestsInStd();
    return 0;
}