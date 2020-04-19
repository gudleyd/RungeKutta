#include <iostream>

#include "src/expression/Expression.h"

int main() {
    Expression p;
    p.parse("-x * x * x + y * y", {"x", "y"});
    std::cout << p.evaluate({2, 3}) << std::endl;
    return 0;
}
