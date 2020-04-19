//
// Created by Ivan on 19.04.2020.
//

/*
 * A little bit about operation precedence
 */

#pragma once

#include <memory>
#include <stack>
#include <cmath>
#include <vector>

enum TokenType {
    Undefined, Number, Function, Operator, LeftParen, RightParen, Variable
};

enum TokenAssociativity {
    Both, Right, Left
};

class Token {
public:
    [[nodiscard]] virtual TokenType type() const { return Undefined; };
    [[nodiscard]] virtual int precedence() const { return -1; }
    [[nodiscard]] virtual TokenAssociativity associativity() const { return Both; }
    virtual void evaluate(std::stack<double>&) const { }
};

class FunctionToken: public Token {
public:
    [[nodiscard]] TokenType type() const override { return Function; }
};

class OperationToken: public Token {
public:
    [[nodiscard]] TokenType type() const override { return Operator; }
};

class VariableToken: public Token {
public:
    explicit VariableToken(int num): num(num) { }
    [[nodiscard]] TokenType type() const override { return Variable; }
    void evaluate(std::stack<double>& s, const std::vector<double>& vars) const {
        s.push(vars[this->num]);
    }
private:
    int num;
};

class LeftParenToken: public Token {
public:
    [[nodiscard]] TokenType type() const override { return LeftParen; }
};

class RightParenToken: public Token {
public:
    [[nodiscard]] TokenType type() const override { return RightParen; }
};

class NumberToken: public Token {
public:
    explicit NumberToken(double value): value(value) {}
    [[nodiscard]] TokenType type() const override { return Number; }
    void evaluate(std::stack<double>& s) const override {
        s.push(this->value);
    }
private:
    double value;
};

class SinToken: public FunctionToken {
public:
    void evaluate(std::stack<double>& s) const override {
        double a = s.top();
        s.pop();
        s.push(std::sin(a));
    }
};

class SumToken: public OperationToken {
public:
    [[nodiscard]] TokenAssociativity associativity() const override { return Both; }
    [[nodiscard]] int precedence() const override { return 2; }
    void evaluate(std::stack<double>& s) const override {
        double a = s.top();
        s.pop();
        double b = s.top();
        s.pop();
        s.push(a + b);
    }
};

class SubToken: public OperationToken {
public:
    [[nodiscard]] TokenAssociativity associativity() const override { return Left; }
    [[nodiscard]] int precedence() const override { return 2; }
    void evaluate(std::stack<double>& s) const override {
        double a = s.top();
        s.pop();
        double b = s.top();
        s.pop();
        s.push(b - a);
    }
};

class MulToken: public OperationToken {
public:
    [[nodiscard]] TokenAssociativity associativity() const override { return Both; }
    [[nodiscard]] int precedence() const override { return 1; }
    void evaluate(std::stack<double>& s) const override {
        double a = s.top();
        s.pop();
        double b = s.top();
        s.pop();
        s.push(a * b);
    }
};

class DivToken: public OperationToken {
public:
    [[nodiscard]] TokenAssociativity associativity() const override { return Left; }
    [[nodiscard]] int precedence() const override { return 1; }
    void evaluate(std::stack<double>& s) const override {
        double a = s.top();
        s.pop();
        double b = s.top();
        s.pop();
        s.push(b / a);
    }
};

class MinusToken: public OperationToken {
    [[nodiscard]] TokenAssociativity associativity() const override { return Right; }
    [[nodiscard]] int precedence() const override { return 0; }
    void evaluate(std::stack<double>& s) const override {
        double a = s.top();
        s.pop();
        s.push(-a);
    }
};

bool isLeftAssociative(const std::shared_ptr<Token>& t);