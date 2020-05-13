//
// Created by Ivan on 21.04.2020.
//

#pragma once

#include <vector>
#include <memory>

#include "../expression/Expression.h"
#include "../utils/utils.h"

namespace rk {

    // Edited by TV on 10.05.2020
    template<typename Value>
    std::vector<Value> RKMasterSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h,
                                const std::vector<std::vector<Value>> &butcherTable) {
        auto diff = (at - initValues[0]);
        if (fabs(diff) < h)
            return std::move(initValues);
        else if (diff < 0)
            throw std::invalid_argument("RK methods do not compute solutions at points left of initValue");
        uint64_t n = (uint64_t)(((long double)diff / h) + 0.5);
        std::vector<std::vector<Value>> k(functions.size(), std::vector<Value>(butcherTable.size() - 1));
        std::vector<Value> tmpValues(initValues);
        for (uint64_t i = 1; i <= n; ++i) {
            for (size_t j = 0; j < butcherTable.size() - 1; ++j) {
                tmpValues[0] = initValues[0] + h * butcherTable[j][0];
                for (size_t t = 1; t <= functions.size(); ++t) {
                    tmpValues[t] = initValues[t];
                    for (size_t j1 = 0; j1 < j; ++j1)
                        tmpValues[t] += k[t - 1][j1] * butcherTable[j][j1 + 1];
                }
                for (size_t t = 0; t < functions.size(); ++t)
                    k[t][j] = h * functions[t]->evaluate(tmpValues);
            }
            initValues[0] += h;
            for (size_t t = 1; t <= functions.size(); ++t) {
                for (size_t j = 0; j < butcherTable.size() - 1; ++j)
                    initValues[t] += k[t - 1][j] * butcherTable[butcherTable.size() - 1][j + 1];
            }
        }
        return std::move(initValues);
    }

    // Edited by TV on 10.05.2020
    template<typename Value>
    std::vector<Value> RKMasterSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h,
                                const std::vector<std::vector<Value>> &butcherTable) {
        std::vector<std::shared_ptr<Expression<Value>>> tmp = { std::make_shared<Expression<Value>>(function) };
        initValues = (RKMasterSystemSolve<Value>(tmp, initValues, at, h, butcherTable));
        return std::move(initValues);
    }

    // Edited by TV on 13.05.2020
    template<typename Value>
    std::vector<Value> ASRKMasterSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value eps,
                                const std::vector<std::vector<Value>> &butcherTable) {
        auto diff = (at - initValues[0]);
        if (fabs(diff) < eps * eps)
            return std::move(initValues);
        else if (diff < 0)
            throw std::invalid_argument("RK methods do not compute solutions at points left of initValue");
        uint64_t n = 0;
        long double h = diff;
        std::vector<std::vector<Value>> k(functions.size(), std::vector<Value>(butcherTable.size() - 1));
        std::vector<Value> tmpValues(initValues);
        std::vector<Value> tmpY(initValues);
        double mDiff = std::numeric_limits<double>::infinity();
        while(fabs(at - initValues[0]) >= eps || mDiff > eps) {
            // Use butcherTable.size() - 2 since the bottom most 2 are for y value coef-s
            for (size_t j = 0; j < butcherTable.size() - 2; ++j) {
                tmpValues[0] = initValues[0] + h * butcherTable[j][0];
                for (size_t t = 1; t <= functions.size(); ++t) {
                    tmpValues[t] = initValues[t];
                    for (size_t j1 = 0; j1 < j; ++j1)
                        tmpValues[t] += k[t - 1][j1] * butcherTable[j][j1 + 1];
                }
                for (size_t t = 0; t < functions.size(); ++t)
                    k[t][j] = h * functions[t]->evaluate(tmpValues);
            }
            tmpValues[0] = initValues[0] + h;
            for (size_t t = 1; t <= functions.size(); ++t) {
                for (size_t j = 0; j < butcherTable.size() - 3; ++j)
                    tmpValues[t] = initValues[t] + k[t - 1][j] * butcherTable[butcherTable.size() - 1][j + 1];
            }

            tmpY[0] = initValues[0] + h;
            for (size_t t = 1; t <= functions.size(); ++t) {
                for (size_t j = 0; j < butcherTable.size() - 2; ++j)
                    tmpY[t] = initValues[t] + k[t - 1][j] * butcherTable[butcherTable.size() - 2][j + 1];
            }
            
            ++n;
            if (!(n & 1)) {
                // On even steps we actually move forward
                initValues = tmpY;
                // Prevent overflows and shit
                if (n >= UINT64_MAX - 1)
                    n = 0;

            } else {
                // On odd steps we adjust our step value
                // Adapt the step size
                mDiff = 0;
                for (size_t j = 0; j < initValues.size(); ++j) {
                    auto tmpDiff = fabs(initValues[j] - tmpY[j]);
                    if (tmpDiff > mDiff)
                        mDiff = tmpDiff;
                }
                h *= 0.9 * std::min(2.0, std::max(0.3, (double)std::sqrt((long double)eps/(2 * mDiff))));
                if (h < 0.000001)
                    h = 0.000001;
            }
        }
        return std::move(initValues);
    }
    // Edited by TV on 12.05.2020
    template<typename Value>
    std::vector<Value> ASRKMasterSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value eps,
                                const std::vector<std::vector<Value>> &butcherTable) {
        std::vector<std::shared_ptr<Expression<Value>>> tmp = { std::make_shared<Expression<Value>>(function) };
        initValues = ASRKMasterSystemSolve<Value>(tmp, initValues, at, eps, butcherTable);
        return std::move(initValues);
    }



    //
    // |                            |
    // |    Convinience functions   |
    // |                            |
    //

    ///////////////////////
    //                   //
    //      ORDER 3      //
    //                   //
    ///////////////////////

    // Edited by TV 11.05.2020
    template<typename Value>
    std::vector<Value> RK3Solve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {0.5, 0.5, 0}, {1, -1, 2, 0}, {0, 1.0/6, 2.0/3, 1.0/6}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 11.05.2020
    template<typename Value>
    std::vector<Value> RK3GenericSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001, 
                                Value alpha = 0.5) {
        if (fabs(alpha) < 0.00001 || fabs(alpha - 2.0/3.0) < 0.00001 || fabs(alpha - 1) < 0.00001)
            throw std::invalid_argument("Alpha Value for Generic RK3 must not be equal 0, 2/3 or 1");
        const auto q = (1 - alpha)/ (alpha * (3.0 * alpha - 2));
        const std::vector<std::vector<Value>> bT({{0, 0}, 
                                                 {alpha, alpha, 0}, 
                                                 {1, 1 + q, -q, 0}, 
                                                 {0, 0.5 - 1.0/(6 * alpha), 
                                                  1/(6 * alpha * (1 - alpha)), 
                                                  (2 - 3 * alpha)/(6 * (1 - alpha))}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 09.05.2020
    template<typename Value>
    std::vector<Value> SSPRK3Solve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {1, 1, 0}, {0.5, 0.5, 0.25, 0}, {0, 1.0/6, 1.0/6, 2.0/3}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 12.05.2020
    template<typename Value>
    std::vector<Value> RK3HeunSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {1.0/3, 1.0/3, 0}, {2.0/3, 0,  2.0/3, 0}, {0, 0.25, 0, 0.25}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 12.05.2020
    template<typename Value>
    std::vector<Value> RK3RalstonSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {0.5, 0.5, 0}, {3.0/4, 0,  3.0/4, 0}, {0, 2.0/9, 1.0/3, 4.0/9}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    ///////////////////////
    //                   //
    //      ORDER 4      //
    //                   //
    ///////////////////////

    // Edited by TV 09.05.2020
    template<typename Value>
    std::vector<Value> RK4Solve(const Expression<Value>& function,
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
            throw std::invalid_argument("RK methods do not compute solutions at points left of initValue");
        uint64_t n = (uint64_t)(((long double)diff / h) + 0.5);
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
    // Edited by TV 09.05.2020
    template<typename Value>
    std::vector<Value> RK4SolveSystem(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        // Same Edit as for RKSolve
        auto diff = (at - initValues[0]);
        if (fabs(diff) < h)
            return std::move(initValues);
        else if (diff < 0)
            throw std::invalid_argument("RK methods do not compute solutions at points left of initValue");
        uint64_t n = (uint64_t)(((long double)diff / h) + 0.5);
        std::vector<std::vector<Value>> k(functions.size(), std::vector<Value>(4));
        std::vector<Value> tmpValues(initValues);
        Value frac = (Value(1) / Value(6));
        for (uint64_t i = 1; i <= n; ++i) {
            for (size_t t = 0; t < functions.size(); ++t)
                k[t][0] = h * functions[t]->evaluate(tmpValues);
            tmpValues[0] += 0.5 * h;
            for (size_t t = 0; t < functions.size(); ++t)
                tmpValues[t + 1] = initValues[t + 1] + 0.5 * k[t][0];
            for (size_t t = 0; t < functions.size(); ++t)
                k[t][1] = h * functions[t]->evaluate(tmpValues);
            for (size_t t = 0; t < functions.size(); ++t)
                tmpValues[t + 1] = initValues[t + 1] + 0.5 * k[t][1];
            for (size_t t = 0; t < functions.size(); ++t)
                k[t][2] = h * functions[t]->evaluate(tmpValues);
            tmpValues[0] += 0.5 * h;
            for (size_t t = 0; t < functions.size(); ++t)
                tmpValues[t + 1] = initValues[t + 1] +  k[t][2];
            for (size_t t = 0; t < functions.size(); ++t)
                k[t][3] = h * functions[t]->evaluate(tmpValues);
            for (size_t t = 0; t < functions.size(); ++t) {
                initValues[t + 1] = initValues[t + 1] +  frac * (k[t][0] + 2 * k[t][1] + 2 * k[t][2] + k[t][3]);
            }
            initValues[0] = tmpValues[0];
        }
        return std::move(initValues);
    }

    // Edited by TV 12.05.2020
    template<typename Value>
    std::vector<Value> RK4ClassicSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,     0}, 
            {1.0/3, 1.0/3,  0}, 
            {2.0/3, -1.0/3, 1,      0},
            {1,     1,      -1,     1,      0},
            {0,     1.0/8,  3.0/8,  3.0/8,  1.0/8}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 12.05.2020
    template<typename Value>
    std::vector<Value> RK4RalstonSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,           0}, 
            {0.4,         0.4,         0}, 
            {0.45573725,  0.29697761,  0.15875964,  0},
            {1,           0.21810040,  -3.05096516, 3.83286476, 0},
            {0,           0.17476028,  -0.55148066, 1.20553560, 0.17118478}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    
    template<typename Value>
    std::vector<Value> ASRKFehlbergSolve(const Expression<Value>& function,
                               std::vector<Value> initValues,
                               Value at,
                               Value eps) {
    const std::vector<std::vector<Value>> bT({
        {0,         0},
        {0.25,      0.25,           0},
        {3.0/8,     3.0/32,         9.0/32,         0},
        {12.0/13,   1932.0/2197,    -7200.0/2197,   7296.0/2197,    0},
        {1,         439.0/216,      -8,             3680.0/513,     -845.0/4104,    0},
        {0.5,       -8.0/27,        2,              -3544.0/2565,   1859.0/4104,    -11.0/40},
        {0,         16.0/135,       0,              6656.0/12825,   28561.0/56430	-9.0/50,        2.0/55},
        {0,         25.0/216,	    0,	            1408.0/2565,	2197.0/4104,	-1.0/5,     	0}
    });
    return ASRKMasterSolve<Value>(function, initValues, at, eps, bT);
}

}
