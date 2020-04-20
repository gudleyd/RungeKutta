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


/*
 * Base Token Class
 */
template<typename Value>
class Token {
public:
    [[nodiscard]] virtual TokenType type() const { return Undefined; };
    [[nodiscard]] virtual int precedence() const { return -1; }
    [[nodiscard]] virtual TokenAssociativity associativity() const { return Both; }
    virtual void evaluate(std::stack<Value>&, const std::vector<Value>&) const { }
};


template<typename Value>
class FunctionToken: public Token<Value> {
public:
    [[nodiscard]] TokenType type() const override { return Function; }
};


template<typename Value>
class OperationToken: public Token<Value> {
public:
    [[nodiscard]] TokenType type() const override { return Operator; }
};


template<typename Value>
class LeftParenToken: public Token<Value> {
public:
    [[nodiscard]] TokenType type() const override { return LeftParen; }
};


template<typename Value>
class RightParenToken: public Token<Value> {
public:
    [[nodiscard]] TokenType type() const override { return RightParen; }
};


template<typename Value>
class VariableToken: public Token<Value> {
public:
    explicit VariableToken(int num): num(num) { }
    [[nodiscard]] TokenType type() const override { return Variable; }
    void evaluate(std::stack<Value>& s, const std::vector<Value>& vars) const {
        s.push(vars[this->num]);
    }
private:
    int num;
};


template<typename Value>
class NumberToken: public Token<Value> {
public:
    explicit NumberToken(Value value): value(value) {}
    [[nodiscard]] TokenType type() const override { return Number; }
    void evaluate(std::stack<Value>& s, const std::vector<Value>& vars) const override {
        s.push(this->value);
    }
private:
    Value value;
};

/*
 * Operation Tokens
 */
template<typename Value>
class SumToken: public OperationToken<Value> {
public:
    [[nodiscard]] TokenAssociativity associativity() const override { return Both; }
    [[nodiscard]] int precedence() const override { return 2; }
    void evaluate(std::stack<Value>& s, const std::vector<Value>& vars) const override {
        Value a = s.top();
        s.pop();
        Value b = s.top();
        s.pop();
        s.push(a + b);
    }
};


template<typename Value>
class SubToken: public OperationToken<Value> {
public:
    [[nodiscard]] TokenAssociativity associativity() const override { return Left; }
    [[nodiscard]] int precedence() const override { return 2; }
    void evaluate(std::stack<Value>& s, const std::vector<Value>& vars) const override {
        Value a = s.top();
        s.pop();
        Value b = s.top();
        s.pop();
        s.push(b - a);
    }
};


template<typename Value>
class MulToken: public OperationToken<Value> {
public:
    [[nodiscard]] TokenAssociativity associativity() const override { return Both; }
    [[nodiscard]] int precedence() const override { return 1; }
    void evaluate(std::stack<Value>& s, const std::vector<Value>& vars) const override {
        Value a = s.top();
        s.pop();
        Value b = s.top();
        s.pop();
        s.push(a * b);
    }
};


template<typename Value>
class DivToken: public OperationToken<Value> {
public:
    [[nodiscard]] TokenAssociativity associativity() const override { return Left; }
    [[nodiscard]] int precedence() const override { return 1; }
    void evaluate(std::stack<Value>& s, const std::vector<Value>& vars) const override {
        Value a = s.top();
        s.pop();
        Value b = s.top();
        s.pop();
        s.push(b / a);
    }
};


template<typename Value>
class UnaryMinusToken: public OperationToken<Value> {
    [[nodiscard]] TokenAssociativity associativity() const override { return Right; }
    [[nodiscard]] int precedence() const override { return 0; }
    void evaluate(std::stack<Value>& s, const std::vector<Value>& vars) const override {
        Value a = s.top();
        s.pop();
        s.push(-a);
    }
};


/*
 * FUNCTION TOKENS
 */

template<typename Value>
class SinToken: public FunctionToken<Value> {
public:
    void evaluate(std::stack<Value>& s, const std::vector<Value>& vars) const override {
        Value a = s.top();
        s.pop();
        s.push(std::sin(a));
    }
};


/*
 * Util Functions
 */

template<typename Value>
bool isLeftAssociative(const std::shared_ptr<Token<Value>>& t) {
    return t->associativity() == Both || t->associativity() == Left;
}

template<typename Value>
bool isParen(const std::shared_ptr<Token<Value>>& t) {
    return t->type() == LeftParen || t->type() == RightParen;
}