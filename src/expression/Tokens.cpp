//
// Created by Ivan on 19.04.2020.
//

#include "Tokens.h"

bool isLeftAssociative(const std::shared_ptr<Token>& t) {
    return t->associativity() == Both || t->associativity() == Left;
}