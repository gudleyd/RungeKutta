//
// Created by Ivan on 19.04.2020.
//

#include "Expression.h"

/*template class Expression<float>;
template class Expression<double>;*/
template class Expression<long double>;


template<typename Value>
const std::map<std::string, std::shared_ptr<Token<Value>>> Expression<Value>::tokens = {
        {"*", std::make_shared<MulToken<Value>>()},
        {"+", std::make_shared<SumToken<Value>>()},
        {"-", std::make_shared<SubToken<Value>>()},
        {"--", std::make_shared<UnaryMinusToken<Value>>()},
        {"/", std::make_shared<DivToken<Value>>()},
        {"(", std::make_shared<LeftParenToken<Value>>()},
        {")", std::make_shared<RightParenToken<Value>>()},
        {"sin", std::make_shared<SinToken<Value>>()}
};


template<typename Value>
void Expression<Value>::parse(std::string s,
        const std::vector<std::string>& variables,
        std::pair<Value, bool> (*f)(const std::string&)) {
    this->vars = variables;
    this->converter = f;
    this->compiled = nullptr;
    this->dll = nullptr;

    this->expression.clear();
    this->mainQueue.clear();

    this->tokenize(s, this->expression);

    std::stack<std::shared_ptr<Token<Value>>> opStack;
    for (auto& t: this->expression) {
        if (t->type() == Number || t->type() == Variable) {
            mainQueue.push_back(t);
        } else if (t->type() == LeftParen || t->type() == Function) {
            opStack.push(t);
        } else if (t->type() == Operator) {
            while ((!opStack.empty()) && (opStack.top()->type() == Function
                                          || (opStack.top()->type() == Operator && (opStack.top()->precedence() < t->precedence() ||
                                                                                    (opStack.top()->precedence() == t->precedence() && isLeftAssociative(t)))))
                   && (opStack.top()->type() != LeftParen)) {
                mainQueue.push_back(opStack.top());
                opStack.pop();
            }
            opStack.push(t);
        } else if (t->type() == RightParen) {
            while ((!opStack.empty()) && opStack.top()->type() != LeftParen) {
                mainQueue.push_back(opStack.top());
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
        mainQueue.push_back(opStack.top());
        opStack.pop();
    }
}


template<typename Value>
Value Expression<Value>::evaluate(const std::vector<Value>& varsValues) {
    if (this->compiled != nullptr) {
        return this->compiled(varsValues.data());
    }
    std::stack<Value> s;
    for (auto& t: mainQueue) {
        t->evaluate(s, varsValues);
    }
    return s.top();
}


template<typename Value>
void Expression<Value>::tokenize(std::string& s, std::vector<std::shared_ptr<Token<Value>>>& v) {
    Expression::prepareString(s);
    std::istringstream iss(s);
    std::vector<std::string> results(std::istream_iterator<std::string>{iss},
                                     std::istream_iterator<std::string>());
    bool neg = false;
    for (size_t i = 0; i < results.size(); ++i) {
        if (results[i] == "-") {
            if (i == results.size() - 1) {
                throw std::logic_error(
                        "Parsing Error:\n\t\tWrong syntax at pos " + std::to_string(results.size() - 1) + "\n");
            }

            if (i == 0 || v.back()->type() == Operator || v.back()->type() == LeftParen) {
                auto token = Expression::getToken("--");
                v.emplace_back(token);
                continue;
            }
        }
        auto token = Expression::getToken(results[i]);
        v.emplace_back(token);
    }
}


template<typename Value>
void Expression<Value>::prepareString(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    for (auto& [tokenName, token]: Expression::tokens) {
        utils_rk::replace(s, tokenName, " " + tokenName + " ");
    }
}


template<typename Value>
std::shared_ptr<Token<Value>> Expression<Value>::getToken(const std::string& tokenName) {

    auto [value, isNum] = this->converter(tokenName);
    if (isNum) {
        return std::make_shared<NumberToken<Value>>(value);
    } else if (Expression::tokens.find(tokenName) != Expression::tokens.end()) {
        const auto& token = Expression::tokens.at(tokenName);
        return token;
    } else {
        auto it = std::find(this->vars.begin(), this->vars.end(), tokenName);
        if (it != this->vars.end()) {
            return std::make_shared<VariableToken<Value>>(it - this->vars.begin());
        }
    }
    throw std::logic_error("Parsing Error:\n\t\tFound unknown token: " + tokenName + "\n");
}


template<typename Value>
bool Expression<Value>::compile() {

    // ToDO:: Insert valid spaces (2--3 - is not valid in c++)
    std::string functionString;
    for (auto& t: this->expression) {
        functionString += t->cname();
    }

    std::string valueName = utils_rk::typeNameToString(typeid(Value).name());
    std::ofstream sf("file.cc");
    sf  << "#include<math.h>\n"
        << "#ifdef __cplusplus\n"
        << "extern \"C\" {\n"
        << "#endif\n"
        << valueName << " compiled(const " << valueName << "* vars) {\n"
        << "return " << functionString << ";\n"
        << "}\n"
        << "#ifdef __cplusplus\n"
        << "}\n"
        << "#endif";
    sf.close();

    system("c++ file.cc -o file.so -shared -fPIC");

#ifndef WIN32
    this->dll = dlopen("file.so", RTLD_LAZY);
#else
    this->dll = LoadLibrary("file.so");
#endif

    if (this->dll == nullptr) return false;

#ifndef WIN32
    fun = (Value (*)(const Value*))dlsym(this->dll, "compiled");
#else
    compiled = (Value (*)(const Value*))GetProcAddress((HINSTANCE)this->dll, "compiled");
#endif

    return (this->compiled != nullptr);
}

template<typename Value>
Expression<Value>::~Expression() {
#ifndef WIN32
    dlclose(this->dll);
#else
    FreeLibrary((HINSTANCE)this->dll);
#endif
}