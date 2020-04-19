//
// Created by Ivan on 19.04.2020.
//

#include <cstring>
#include "Parser.h"

bool isLeftAssociative(const std::shared_ptr<Token>& t) {
    return t->associativity() == Both || t->associativity() == Left;
}

std::pair<double, bool> stringToDouble(const std::string& s) {
    char* p;
    double converted = std::strtod(s.c_str(), &p);
    if (*p)
        return {0, 0};
    else
        return {converted, 1};
}

const std::map<std::string, std::shared_ptr<Token>> Parser::tokens = {
        {"*", std::make_shared<MulToken>()},
        {"+", std::make_shared<SumToken>()},
        {"-", std::make_shared<SubToken>()},
        {"--", std::make_shared<MinusToken>()},
        {"/", std::make_shared<DivToken>()},
        {"(", std::make_shared<LeftParenToken>()},
        {")", std::make_shared<RightParenToken>()},
        {"sin", std::make_shared<SinToken>()}
};


void Parser::parse(std::string s) {
    while (!mainQueue.empty()) {
        mainQueue.pop();
    }

    std::vector<std::shared_ptr<Token>> v;
    Parser::prepareTokenVector(s, v);

    std::stack<std::shared_ptr<Token>> opStack;
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i]->type() == Number) {
            mainQueue.push(v[i]);
        } else if (v[i]->type() == Function) {
            opStack.push(v[i]);
        } else if (v[i]->type() == Operator) {
            while ((!opStack.empty()) && (opStack.top()->type() == Function
                                          || (opStack.top()->type() == Operator && (opStack.top()->precedence() < v[i]->precedence() ||
                                                                                    (opStack.top()->precedence() == v[i]->precedence() && isLeftAssociative(v[i])))))
                   && (opStack.top()->type() != LeftParen)) {
                mainQueue.push(opStack.top());
                opStack.pop();
            }
            opStack.push(v[i]);
        } else if (v[i]->type() == LeftParen) {
            opStack.push(v[i]);
        } else if (v[i]->type() == RightParen) {
            while ((!opStack.empty()) && opStack.top()->type() != LeftParen) {
                mainQueue.push(opStack.top());
                opStack.pop();
            }
            if (!opStack.empty() && opStack.top()->type() == LeftParen) {
                opStack.pop();
            }
        }
    }
    while (!opStack.empty()) {
        mainQueue.push(opStack.top());
        opStack.pop();
    }
}

double Parser::evaluate() {
    std::stack<double> s;
    while (!mainQueue.empty()) {
        auto t = mainQueue.front();
        t->evaluate(s);
        mainQueue.pop();
    }
    return s.top();
}

void Parser::prepareTokenVector(std::string& s, std::vector<std::shared_ptr<Token>>& v) {
    Parser::prepareString(s);
    std::istringstream iss(s);
    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());
    bool neg = false;
    for (size_t i = 0; i < results.size(); ++i) {
        if (results[i] == "-") {
            if (i == results.size() - 1) {
                // ToDO: throw error
            }

            if (i == 0 || v.back()->type() == Operator || v.back()->type() == LeftParen) {
                auto token = Parser::getToken("--");
                v.emplace_back(token);
            } else {
                auto token = Parser::getToken("-");
                v.emplace_back(token);
            }
        } else {
            auto token = Parser::getToken(results[i]);
            v.emplace_back(token);
        }
    }
}

void Parser::prepareString(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    for (auto& [tokenName, token]: Parser::tokens) {
        Parser::replace(s, tokenName, " " + tokenName + " ");
    }
}

void Parser::replace(std::string& s, const std::string& from, const std::string& to) {

    size_t pos = s.find(from);
    while( pos != std::string::npos)
    {
        s.replace(pos, from.size(), to);
        pos = s.find(from, pos + to.size());
    }
}

std::shared_ptr<Token> Parser::getToken(const std::string& tokenName) {
    // ToDO: Add variables

    auto [value, isNum] = stringToDouble(tokenName);
    if (isNum) {
        return std::make_shared<NumberToken>(value);
    } else {
        const auto& token = Parser::tokens.at(tokenName);
        return token;
    }
}