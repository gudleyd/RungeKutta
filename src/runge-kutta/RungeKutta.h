//
// Created by Ivan on 21.04.2020.
//

#pragma once

#include <vector>
#include <memory>

#include "../expression/Expression.h"
#include "../utils/utils.h"

namespace rk {

    template<typename Value>
    Value RK_solve_one(const Expression<Value>& function,
                        std::vector<Value> initValues,
                        Value at,
                        Value h = 0.001) {
        auto n = (size_t)((at - initValues[0]) / h);
        Value k1, k2, k3, k4;
        for (size_t i = 1; i <= n; ++i) {
            k1 = h * function.evaluate(initValues);
            initValues[0] += 0.5 * h;
            initValues[1] += 0.5 * k1;
            k2 = h * function.evaluate(initValues);
            initValues[1] += 0.5 * k2 - 0.5 * k1;
            k3 = h * function.evaluate(initValues);
            initValues[0] += 0.5 * h;
            initValues[1] += k3 - 0.5 * k2;
            k4 = h * function.evaluate(initValues);
            initValues[1] += (Value(1) / Value(6)) * (k1 + 2 * k2 + 2 * k3 + k4) - k3;
        }
        return initValues[1];
    }
}
