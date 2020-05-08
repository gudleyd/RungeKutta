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
    std::vector<Value> RKSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        auto diff = (at - initValues[0]); 
        /* 
            This is needed in cases where there is no reason to solve
            since combined float error will just result in a random result
            instead of an actual solution 
            (e.g. 1.0... - 1.0... < 0 and we run for UINT32_MAX iterations for no reason)
        */
        if (fabs(diff) < h)
            return std::move(initValues);
        // Algorithm does not apply in this case
        else if (diff < 0)
            throw std::invalid_argument("Basic RKSolve method does not compute solutions at points left of initValue");
        uint64_t n = (uint64_t)(diff / h) + 1;
        /*
            Cuts off cases where the algorithm doesn't converge propperly

            With so many iterations it is gurantied that the
            result won't be an actual solution
        */
        if (n > (1llu << 20)) {
            n = 1llu << 20;
        }
        Value k1, k2, k3, k4;
        Value frac = (Value(1) / Value(6));
        for (uint64_t i = 1; i <= n; ++i) {
            k1 = h * function.evaluate(initValues);
            initValues[0] += 0.5 * h;
            initValues[1] += 0.5 * k1;
            k2 = h * function.evaluate(initValues);
            initValues[1] += 0.5 * k2 - 0.5 * k1;
            k3 = h * function.evaluate(initValues);
            initValues[0] += 0.5 * h;
            initValues[1] += k3 - 0.5 * k2;
            k4 = h * function.evaluate(initValues);
            initValues[1] += frac * (k1 + 2 * k2 + 2 * k3 + k4) - k3;
        }
        return std::move(initValues);
    }

    template<typename Value>
    std::vector<Value> RKSolveSystem(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        auto n = (size_t)((at - initValues[0]) / h);
        std::vector<std::vector<Value>> k(functions.size(), std::vector<Value>(4));
        Value frac = (Value(1) / Value(6));
        for (size_t i = 1; i <= n; ++i) {
            for (size_t t = 0; t < functions.size(); ++t)
                k[t][0] = h * functions[t]->evaluate(initValues);
            initValues[0] += 0.5 * h;
            for (size_t t = 0; t < functions.size(); ++t)
                initValues[t + 1] += 0.5 * k[t][0];
            for (size_t t = 0; t < functions.size(); ++t)
                k[t][1] = h * functions[t]->evaluate(initValues);
            for (size_t t = 0; t < functions.size(); ++t)
                initValues[t + 1] += 0.5 * k[t][1] - 0.5 * k[t][0];
            for (size_t t = 0; t < functions.size(); ++t)
                k[t][2] = h * functions[t]->evaluate(initValues);
            initValues[0] += 0.5 * h;
            for (size_t t = 0; t < functions.size(); ++t)
                initValues[t + 1] += k[t][2] - 0.5 * k[t][1];
            for (size_t t = 0; t < functions.size(); ++t)
                k[t][3] = h * functions[t]->evaluate(initValues);
            for (size_t t = 0; t < functions.size(); ++t)
                initValues[t + 1] += frac * (k[t][0] + 2 * k[t][1] + 2 * k[t][2] + k[t][3]) - k[t][2];
        }
        return std::move(initValues);
    }
}
