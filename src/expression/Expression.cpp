//
// Created by Ivan on 19.04.2020.
//

#include <cstring>
#include "Expression.h"

std::pair<double, bool> stringToDouble(const std::string& s) {
    char* p;
    double converted = std::strtod(s.c_str(), &p);
    if (*p)
        return {0, 0};
    else
        return {converted, 1};
}

const std::map<std::string, std::shared_ptr<Token>> Expression::tokens = {
        {"*", std::make_shared<MulToken>()},
        {"+", std::make_shared<SumToken>()},
        {"-", std::make_shared<SubToken>()},
        {"--", std::make_shared<MinusToken>()},
        {"/", std::make_shared<DivToken>()},
        {"(", std::make_shared<LeftParenToken>()},
        {")", std::make_shared<RightParenToken>()},
        {"sin", std::make_shared<SinToken>()}
};


void Expression::parse(std::string s, const std::vector<std::string>& variables) {
    this->vars = variables;
    while (!mainQueue.empty()) {
        mainQueue.pop();
    }

    std::vector<std::shared_ptr<Token>> v;
    this->tokenize(s, v);

    std::stack<std::shared_ptr<Token>> opStack;
    for (auto& t: v) {
        if (t->type() == Number || t->type() == Variable) {
            mainQueue.push(t);
        } else if (t->type() == LeftParen || t->type() == Function) {
            opStack.push(t);
        } else if (t->type() == Operator) {
            while ((!opStack.empty()) && (opStack.top()->type() == Function
                                          || (opStack.top()->type() == Operator && (opStack.top()->precedence() < t->precedence() ||
                                                                                    (opStack.top()->precedence() == t->precedence() && isLeftAssociative(t)))))
                   && (opStack.top()->type() != LeftParen)) {
                mainQueue.push(opStack.top());
                opStack.pop();
            }
            opStack.push(t);
        } else if (t->type() == RightParen) {
            while ((!opStack.empty()) && opStack.top()->type() != LeftParen) {
                mainQueue.push(opStack.top());
                opStack.pop();
            }
            if (opStack.empty())
                throw std::logic_error("Parsing Error:\n\t\tMismatched parentheses\n");
            if (opStack.top()->type() == LeftParen) {
                opStack.pop();
            }
        }
    }
    while (!opStack.empty()) {
        if (isParen(opStack.top()))
            throw std::logic_error("Parsing Error:\n\t\tMismatched parentheses\n");
        mainQueue.push(opStack.top());
        opStack.pop();
    }
}

double Expression::evaluate(const std::vector<double>& varsValues) {
    std::stack<double> s;
    while (!mainQueue.empty()) {
        auto t = mainQueue.front();
        if (t->type() == Variable) {
            auto v = std::dynamic_pointer_cast<VariableToken>(t);
            v->evaluate(s, varsValues);
        } else {
            t->evaluate(s);
        }

        mainQueue.pop();
    }
    return s.top();
}

void Expression::tokenize(std::string& s, std::vector<std::shared_ptr<Token>>& v) {
    Expression::prepareString(s);
    std::istringstream iss(s);
    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());
    bool neg = false;
    for (size_t i = 0; i < results.size(); ++i) {
        if (results[i] == "-") {
            if (i == results.size() - 1) {
                throw std::logic_error("Parsing Error:\n\t\tWrong syntax at pos " + std::to_string(results.size() - 1) + "\n");
            }

            if (i == 0 || v.back()->type() == Operator || v.back()->type() == LeftParen) {
                auto token = Expression::getToken("--");
                v.emplace_back(token);
            } else {
                auto token = Expression::getToken("-");
                v.emplace_back(token);
            }
        } else {
            auto token = Expression::getToken(results[i]);
            v.emplace_back(token);
        }
    }
}

void Expression::prepareString(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    for (auto& [tokenName, token]: Expression::tokens) {
        utils_rk::replace(s, tokenName, " " + tokenName + " ");
    }
}

std::shared_ptr<Token> Expression::getToken(const std::string& tokenName) {

    auto [value, isNum] = stringToDouble(tokenName);
    if (isNum) {
        return std::make_shared<NumberToken>(value);
    } else if (Expression::tokens.find(tokenName) != Expression::tokens.end()) {
        const auto& token = Expression::tokens.at(tokenName);
        return token;
    } else {
        auto it = std::find(this->vars.begin(), this->vars.end(), tokenName);
        if (it != this->vars.end()) {
            return std::make_shared<VariableToken>(it - this->vars.begin());
        }
    }
    throw std::logic_error("Parsing Error:\n\t\tFound unknown token: " + tokenName + "\n");
}