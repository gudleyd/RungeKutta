#include <iostream>

#include "test/RunTests.h"

void runTestsInStd() {
    tests_rk::runTests(std::cout, true);
}

int main() {
    runTestsInStd();
    return 0;
}

