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

class Expression {
public:
    void parse(std::string, const std::vector<std::string>& = {});
    double evaluate(const std::vector<double>& = {});
private:
    std::queue<std::shared_ptr<Token>> mainQueue;
    std::vector<std::string> vars;

    void tokenize(std::string&, std::vector<std::shared_ptr<Token>>&);
    std::shared_ptr<Token> getToken(const std::string&);

    static void prepareString(std::string&);
    static const std::map<std::string, std::shared_ptr<Token>> tokens;
};