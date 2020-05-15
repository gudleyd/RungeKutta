# **RungeKutta v0.1.0**

RungeKutta is a library for finding approximate solutions of ordinary differential equations.

## RungeKutta Methods
##### 2nd Order Methods
- RK2HeunSolve
- RK2MidpointSolve
- RK2RalstonSolve
- RK2GenericSolve
##### 3rd Order Methods
- RK3Solve
- RK3GenericSolve
- SSPRK3Solve
- RK3HeunSolve
- RK3RalstonSolve
##### 4th Order Methods
- RK4Solve
- RK4SolveSystem
- RK4ClassicSolve
- RK4RalstonSolve
- SSPRK4Solve
##### 5th Order Methods
- SSPRK5Solve
##### Adaptive Methods
- ASRKBogackiShampineSolve
- ASRKFehlbergSolve
- ASRKCashKarpSolve
- ASRKDormandPrinceSolve
##### Master Functions
- RKMasterSolve
- RKMasterSystemSolve
- ASRKMasterSolve
- ASRKMasterSystemSolve



## Examples
### rk::Expression<Value>
rk::Expression<Value> is a main **template** class for storing and managing functions in RungeKutta library.
All calculations are performed using the type Value.

#### rk::Expression<Value>::parse
Using this method you can create rk::Expression<Value> from string
```cpp
#include <iostream>
#include "RungeKutta.h"

int main() {
    rk::Expression<double> expression;
    expression.parse("6 * 7");
    std::cout << expression.evaluate() << std::endl;
    return 0;
}
```
```bash
~ 42
```
#### rk::Expression<Value>::setFunction
Using this method you can create rk::Expression<Value> from your own function, presented as **Value (\*function)(const Value\*)**.
```cpp
#include <iostream>
#include "RungeKutta.h"

double myFunction(const double* data) {
    return data[0] + data[1];
}

int main() {
    rk::Expression<double> expression;
    expression.setFunction(myFunction);
    std::cout << expression.evaluate({1, 2}) << std::endl;
}
```
```bash
~ 3
```
#### rk::Expression<Value>::compile
Any rk::Expression<Value> can be compiled into byte code, which will magnificently increase performance, up to 1000%.
```cpp
#include <iostream>
#include "RungeKutta.h"

int main() {
    rk::Expression<double> expression;
    expression.parse("x + sin(y)", {"x", "y"});
    expression.compile();
    std::cout << expression.evaluate({1, 2}) << std::endl;
    return 0;
}

```
```bash
~ 5
```
#### rk::Expression<Value>::evaluate
You can evaluate your function, providing all variables values in the very same order as in parse.
```cpp
#include <iostream>
#include "RungeKutta.h"

int main() {
    rk::Expression<double> expression;
    expression.parse("x + sin(y) - cos(x) * 10 + pow(1.5, 3.2)", {"x", "y"});
    std::cout << expression.evaluate({1, 2}) << std::endl;
    return 0;
}

```
```bash
~ 0.166367
```
#### rk::Expression<Value>::addFunctionToken
Maybe, you will also need your own function, used inside expression, for that, you can use this method to add new **<math.h>** function token into token pool, which will be used in parse and compile methods.
```cpp
#include <iostream>
#include "RungeKutta.h"

template<typename Value>
class AddToken: public FunctionToken<Value> {
public:
    void evaluate(std::stack<Value>& s, const std::vector<Value>& vars) const override {
        Value a = s.top();
        s.pop();
        Value b = s.top();
        s.pop();
        s.push(a + b);
    }
    [[nodiscard]] std::string cname() const override { return "add"; }
};

int main() {
    rk::Expression<double> expression;
    rk::Expression<double>::addFunctionToken("add", std::make_shared<AddToken<double>>());
    expression.parse("add(x, y)", {"x", "y"});
    std::cout << expression.evaluate({6, 3}) << std::endl;
    return 0;
}
```
```bash
~ 6
```


