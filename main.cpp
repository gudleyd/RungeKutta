#include <iostream>

#include "src/parser/Expression.h"

int main() {
    Expression p;
    p.parse("2*-(2+4)");
    // Correct Answer 446874382903430
    std::cout << p.evaluate() << std::endl;
    return 0;
}
