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

class Parser {
public:
    void parse(std::string s);
    double evaluate();
private:
    std::queue<std::shared_ptr<Token>> mainQueue;

    static void prepareTokenVector(std::string&, std::vector<std::shared_ptr<Token>>&);
    static void prepareString(std::string&);
    static void replace(std::string&, const std::string&, const std::string&);
    static std::shared_ptr<Token> getToken(const std::string&);

    static const std::map<std::string, std::shared_ptr<Token>> tokens;
};