//
// Created by Ivan on 19.04.2020.
//

#pragma once

#include <stack>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <map>
#include <memory>
#include <queue>

#include "Tokens.h"
#include "../utils/utils.h"

template<typename Value>
class Expression {
public:
    void parse(std::string,
                const std::vector<std::string>& = {},
                std::pair<Value, bool> (*f)(const std::string&) = utils_rk::stringToDouble);
    Value evaluate(const std::vector<Value>& = {});
private:
    std::queue<std::shared_ptr<Token<Value>>> mainQueue;
    std::vector<std::shared_ptr<Token<Value>>> expression;
    std::vector<std::string> vars;
    std::pair<Value, bool> (*converter)(const std::string&) = nullptr;

    void tokenize(std::string&, std::vector<std::shared_ptr<Token<Value>>>&);
    std::shared_ptr<Token<Value>> getToken(const std::string&);

    static void prepareString(std::string&);
    static const std::map<std::string, std::shared_ptr<Token<Value>>> tokens;
};