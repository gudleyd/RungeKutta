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
#include <list>
#include <cstring>
#include <fstream>

#ifndef WIN32
    #include <dlfcn.h>
#else
    #include <windows.h>
#endif

#include "Tokens.h"
#include "../utils/utils.h"

namespace rk {

    template<typename Value>
    class Expression {
    public:
        Expression() = default;
        Expression(const Expression<Value>&);
        Expression<Value>& operator=(Expression<Value> &other);
        ~Expression();

        void parse(std::string,
                   const std::vector<std::string>& = {},
                   std::pair<Value, bool> (*f)(const std::string&) = utils_rk::stringToDouble);
        void setFunction(Value (*function)(const Value*));
        Value evaluate(const std::vector<Value>& = {}) const;
        bool compile();

        static void addFunctionToken(const std::string& name, std::shared_ptr<FunctionToken<Value>> token);
    private:
        bool fromString;
        std::list<std::shared_ptr<Token<Value>>> mainQueue;
        std::vector<std::shared_ptr<Token<Value>>> expression;
        std::vector<std::string> vars;
        std::pair<Value, bool> (*converter)(const std::string&) = nullptr;

        void* dll = nullptr;
        Value (*compiled)(const Value*) = nullptr;
        std::string compileName;

        void tokenize(std::string&, std::vector<std::shared_ptr<Token<Value>>>&);
        std::shared_ptr<Token<Value>> getToken(const std::string&);

        static std::map<std::string, std::shared_ptr<Token<Value>>> tokens;
        static std::map<std::string, size_t> dlls;
    };

}