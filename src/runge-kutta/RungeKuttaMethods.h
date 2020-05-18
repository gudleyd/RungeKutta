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
        
        long double h = diff;
        std::vector<std::vector<Value>> k(functions.size(), std::vector<Value>(butcherTable.size() - 2));
        std::vector<Value> valsHOrder(initValues);
        std::vector<Value> valsLOrder(initValues);
        uint64_t n = 0;
        long double mDiff = std::numeric_limits<double>::infinity();
        while (at - initValues[0] >= eps * eps) {
            // Calculate all k-s
            for (size_t i = 0; i < butcherTable.size() - 2; ++i) {
                valsLOrder[0] = initValues[0] + h * butcherTable[i][0];
                for (size_t j = 1; j <= functions.size(); ++j) {
                    valsLOrder[j] = initValues[j];
                    for (size_t t = 0; t < i; ++t){
                        valsLOrder[j] += k[j - 1][t] * butcherTable[i][t + 1];
                    }
                }
                for (size_t j = 0; j < functions.size(); ++j) 
                    k[j][i] = h * functions[j]->evaluate(valsLOrder); 
            }
            // Calculate Low order and High order vals
            valsLOrder[0] = initValues[0] + h;
            valsHOrder[0] = valsLOrder[0];
            for (size_t j = 1; j <= functions.size(); ++j) {
                valsLOrder[j] = initValues[j];
                valsHOrder[j] = initValues[j];
                for (size_t t = 0; t < butcherTable.size() - 2; ++t) {
                    valsLOrder[j] += k[j - 1][t] * butcherTable[butcherTable.size() - 1][t + 1];
                    valsHOrder[j] += k[j - 1][t] * butcherTable[butcherTable.size() - 2][t + 1];
                }
            }
            
            if ((n & 3) != 3) {
                // Now find mDiff
                mDiff = 0.0L;
                for (size_t j = 1; j <= functions.size(); ++j) {
                    auto tmpDiff = valsHOrder[j] - valsLOrder[j];
                    if (tmpDiff > mDiff)
                        mDiff = tmpDiff;
                    else if (tmpDiff < -mDiff)
                        mDiff = -tmpDiff;
                }
                h *= 0.9L * std::min(2.0L, std::max(0.05L, std::sqrt((long double)eps / (2.0L * mDiff))));
                if (h < 0.0000001L)
                    h = 0.0000001L;  
                long double tmpH = at - initValues[0];
                if (h > tmpH)
                    h = tmpH;
            } else {
                if (n == UINT64_MAX)
                    throw std::runtime_error("Unable to finish solving for specified values");
                initValues = valsHOrder;
            }
            ++n;
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
    // |    Convenience functions   |
    // |                            |
    //


    ///////////////////////
    //                   //
    //      ORDER 2      //
    //                   //
    ///////////////////////

    // Edited by TV 14.05.2020
    template<typename Value>
    std::vector<Value> RK2MidpointSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.00001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {0.5, 0.5, 0}, {0, 0, 1}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    template<typename Value>
    std::vector<Value> RK2MidpointSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.00001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {0.5, 0.5, 0}, {0, 0, 1}});
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    // Edited by TV 14.05.2020
    template<typename Value>
    std::vector<Value> RK2HeunSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.00001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {1, 1, 0}, {0, 0.5, 0.5}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }
    
    template<typename Value>
    std::vector<Value> RK2HeunSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.00001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {1, 1, 0}, {0, 0.5, 0.5}});
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    // Edited by TV 14.05.2020
    template<typename Value>
    std::vector<Value> RK2RalstonSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.00001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {2.0/3, 2.0/3, 0}, {0, 0.25, 0.75}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }
    
    template<typename Value>
    std::vector<Value> RK2RalstonSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.00001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {2.0/3, 2.0/3, 0}, {0, 0.25, 0.75}});
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    // Edited by TV 14.05.2020
    // By default uses same alpha as Ralstons method
    template<typename Value>
    std::vector<Value> RK2GenericSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.00001,
                                Value alpha = 2.0/3) {
        const Value dAlph = 2 * alpha;
        if (fabs(alpha) < 0.000001)
            throw std::invalid_argument("Alpha Value for Generic RK2 must not be equal 0");
        const std::vector<std::vector<Value>> bT({{0, 0}, {alpha, alpha, 0}, {0, 1 - 1/dAlph, 1/(dAlph)}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }
    // By default uses same alpha as Ralstons method
    template<typename Value>
    std::vector<Value> RK2GenericSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.00001,
                                Value alpha = 2.0/3) {
        const Value dAlph = 2 * alpha;
        if (fabs(alpha) < 0.000001)
            throw std::invalid_argument("Alpha Value for Generic RK2 must not be equal 0");
        const std::vector<std::vector<Value>> bT({{0, 0}, {alpha, alpha, 0}, {0, 1 - 1/dAlph, 1/(dAlph)}});
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    

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

    // Edited by TV 13.05.2020
    template<typename Value>
    std::vector<Value> RK3SystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({{0, 0}, {0.5, 0.5, 0}, {1, -1, 2, 0}, {0, 1.0/6, 2.0/3, 1.0/6}});
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    // Edited by TV 11.05.2020
    template<typename Value>
    std::vector<Value> RK3GenericSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001, 
                                Value alpha = 0.5) {
        if (fabs(alpha) < 0.000001 || fabs(alpha - 2.0/3.0) < 0.00001 || fabs(alpha - 1) < 0.00001)
            throw std::invalid_argument("Alpha Value for Generic RK3 must not be equal 0, 2/3 or 1");
        const auto q = (1 - alpha)/ (alpha * (3.0 * alpha - 2));
        const std::vector<std::vector<Value>> bT({
            {0,     0}, 
            {alpha, alpha,                  0}, 
            {1,     1 + q,                  -q,                             0}, 
            {0,     0.5 - 1.0/(6 * alpha),  1/(6 * alpha * (1 - alpha)),    (2 - 3 * alpha)/(6 * (1 - alpha))}
        });
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 13.05.2020
    template<typename Value>
    std::vector<Value> RK3GenericSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001, 
                                Value alpha = 0.5) {
        if (fabs(alpha) < 0.000001 || fabs(alpha - 2.0/3.0) < 0.00001 || fabs(alpha - 1) < 0.00001)
            throw std::invalid_argument("Alpha Value for Generic RK3 must not be equal 0, 2/3 or 1");
        const auto q = (1 - alpha)/ (alpha * (3.0 * alpha - 2));
        const std::vector<std::vector<Value>> bT({
            {0,     0}, 
            {alpha, alpha,                  0}, 
            {1,     1 + q,                  -q,                             0}, 
            {0,     0.5 - 1.0/(6 * alpha),  1/(6 * alpha * (1 - alpha)),    (2 - 3 * alpha)/(6 * (1 - alpha))}
        });
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    // Edited by TV 09.05.2020
    template<typename Value>
    std::vector<Value> SSPRK3Solve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,          0}, 
            {1,   1,     0}, 
            {0.5, 0.5,   0.25,  0}, 
            {0,   1.0/6, 1.0/6, 2.0/3}
        });
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 13.05.2020
    template<typename Value>
    std::vector<Value> SSPRK3SystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,          0}, 
            {1,   1,     0}, 
            {0.5, 0.5,   0.25,  0}, 
            {0,   1.0/6, 1.0/6, 2.0/3}});
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    // Edited by TV 12.05.2020
    template<typename Value>
    std::vector<Value> RK3HeunSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,     0}, 
            {1.0/3, 1.0/3,  0}, 
            {2.0/3, 0,      2.0/3,  0}, 
            {0,     0.25,   0,      0.75}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 13.05.2020
    template<typename Value>
    std::vector<Value> RK3HeunSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,     0}, 
            {1.0/3, 1.0/3,  0}, 
            {2.0/3, 0,      2.0/3,  0}, 
            {0,     0.25,   0,      0.75}});
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    // Edited by TV 12.05.2020
    template<typename Value>
    std::vector<Value> RK3RalstonSolve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,     0}, 
            {0.5,   0.5,    0}, 
            {3.0/4, 0,      3.0/4, 0}, 
            {0,     2.0/9,  1.0/3, 4.0/9}});
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 13.05.2020
    template<typename Value>
    std::vector<Value> RK3RalstonSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,     0}, 
            {0.5,   0.5,    0}, 
            {3.0/4, 0,      3.0/4, 0}, 
            {0,     2.0/9,  1.0/3, 4.0/9}});
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
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
    std::vector<Value> RK4SystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
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

    // Edited by TV 14.05.2020
    template<typename Value>
    std::vector<Value> RK4ClassicSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,     0}, 
            {1.0/3, 1.0/3,  0}, 
            {2.0/3, -1.0/3, 1,      0},
            {1,     1,      -1,     1,      0},
            {0,     1.0/8,  3.0/8,  3.0/8,  1.0/8}});
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
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

    // Edited by TV 14.05.2020
    template<typename Value>
    std::vector<Value> RK4RalstonSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,           0}, 
            {0.4,         0.4,         0}, 
            {0.45573725,  0.29697761,  0.15875964,  0},
            {1,           0.21810040,  -3.05096516, 3.83286476, 0},
            {0,           0.17476028,  -0.55148066, 1.20553560, 0.17118478}});
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    // Edited by TV 14.05.2020
    template<typename Value>
    std::vector<Value> SSPRK4Solve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,     0}, 
            {0.5,   0.5,    0}, 
            {1,     0.5,    0.5,    0}, 
            {0.5,   1.0/6,  1.0/6,  1.0/6,  0}, 
            {0,     1.0/6,  1.0/6,  1.0/6,  0.5}
        });
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 14.05.2020
    template<typename Value>
    std::vector<Value> SSPRK4SystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,     0}, 
            {0.5,   0.5,    0}, 
            {1,     0.5,    0.5,    0}, 
            {0.5,   1.0/6,  1.0/6,  1.0/6,  0}, 
            {0,     1.0/6,  1.0/6,  1.0/6,  0.5}
        });
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    ///////////////////////
    //                   //
    //      ORDER 5      //
    //                   //
    ///////////////////////

    // Edited by TV 14.05.2020
    template<typename Value>
    std::vector<Value> SSPRK5Solve(const Expression<Value>& function,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,         0}, 
            {0.37727,   0.37727,    0}, 
            {0.75454,   0.37727,    0.37727,    0}, 
            {0.72899,   0.24300,    0.24300,    0.24300,    0},
            {0.69923,   0.15359,    0.15359,    0.15359,    0.23846,    0},
            {0,         0.20673,    0.20673,    0.11710,    0.18180,    0.28763}
        });
        return RKMasterSolve<Value>(function, initValues, at, h, bT);
    }

    // Edited by TV 14.05.2020
    template<typename Value>
    std::vector<Value> SSPRK5SystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                                std::vector<Value> initValues,
                                Value at,
                                Value h = 0.001) {
        const std::vector<std::vector<Value>> bT({
            {0,         0}, 
            {0.37727,   0.37727,    0}, 
            {0.75454,   0.37727,    0.37727,    0}, 
            {0.72899,   0.24300,    0.24300,    0.24300,    0},
            {0.69923,   0.15359,    0.15359,    0.15359,    0.23846,    0},
            {0,         0.20673,    0.20673,    0.11710,    0.18180,    0.28763}
        });
        return RKMasterSystemSolve<Value>(functions, initValues, at, h, bT);
    }

    ///////////////////////////////
    //                           //
    //       Adaptive step       //
    //                           //
    ///////////////////////////////

    /////////////////////////
    //                     //
    //      ORDER 3|4      //
    //                     //
    /////////////////////////

    template<typename Value>
    std::vector<Value> ASRKBogackiShampineSolve(const Expression<Value>& function,
                               std::vector<Value> initValues,
                               Value at,
                               Value eps) {
        const std::vector<std::vector<Value>> bT({
            {0,     0},
            {0.5,   0.5,    0},
            {0.75,  0,      0.75,   0},
            {1,     2.0/9,  1.0/3,  4.0/9},
            {0,     2.0/9,	1.0/3,	4.0/9,  0},
            {0,     7.0/24,	0.25,	1.0/3,	0.125}
        });
        return ASRKMasterSolve<Value>(function, initValues, at, eps, bT);
    }

    template<typename Value>
    std::vector<Value> ASRKBogackiShampineSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                               std::vector<Value> initValues,
                               Value at,
                               Value eps) {
        const std::vector<std::vector<Value>> bT({
            {0,     0},
            {0.5,   0.5,    0},
            {0.75,  0,      0.75,   0},
            {1,     2.0/9,  1.0/3,  4.0/9},
            {0,     2.0/9,	1.0/3,	4.0/9,  0},
            {0,     7.0/24,	0.25,	1.0/3,	0.125}
        });
        return ASRKMasterSystemSolve<Value>(functions, initValues, at, eps, bT);
    }

    /////////////////////////
    //                     //
    //      ORDER 4|5      //
    //                     //
    /////////////////////////

    
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

    template<typename Value>
    std::vector<Value> ASRKFehlbergSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
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
        return ASRKMasterSystemSolve<Value>(functions, initValues, at, eps, bT);
    }

    template<typename Value>
    std::vector<Value> ASRKCashCarpSolve(const Expression<Value>& function,
                               std::vector<Value> initValues,
                               Value at,
                               Value eps) {
        const std::vector<std::vector<Value>> bT({
            {0L,         0L},
            {0.2L,       0.2L,            0L},
            {0.3L,       3.0L/40,         9.0L/40,         0L},
            {0.6L,       0.3L,            -0.9L,           1.2L,            0L},
            {1L,         -11.0L/54,       2.5L,            -70.0L/27,       35.0L/27,        0L},
            {7.0L/8,     1631.0L/55296,	  175.0L/512,	   575.0L/13824,    44275.0L/110592, 253.0L/4096},
            {0L,         37.0L/378,	      0L,	           250.0L/621,	    125.0L/594,    	 0L,              512.0L/1771},
            {0L,         2825.0L/27648L,  0L,	           18575.0L/48384,	13525.0L/55296,	 277.0L/14336,	  0.25L}
        });
        return ASRKMasterSolve<Value>(function, initValues, at, eps, bT);
    }

    template<typename Value>
    std::vector<Value> ASRKCashCarpSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                               std::vector<Value> initValues,
                               Value at,
                               Value eps) {
        const std::vector<std::vector<Value>> bT({
            {0L,         0L},
            {0.2L,       0.2L,            0L},
            {0.3L,       3.0L/40,         9.0L/40,         0L},
            {0.6L,       0.3L,            -0.9L,           1.2L,            0L},
            {1L,         -11.0L/54,       2.5L,            -70.0L/27,       35.0L/27,        0L},
            {7.0L/8,     1631.0L/55296,	  175.0L/512,	   575.0L/13824,    44275.0L/110592, 253.0L/4096},
            {0L,         37.0L/378,	      0L,	           250.0L/621,	    125.0L/594,    	 0L,              512.0L/1771},
            {0L,         2825.0L/27648L,  0L,	           18575.0L/48384,	13525.0L/55296,	 277.0L/14336,	  0.25L}
        });
        return ASRKMasterSystemSolve<Value>(functions, initValues, at, eps, bT);
    }

    template<typename Value>
    std::vector<Value> ASRKDormandPrinceSolve(const Expression<Value>& function,
                               std::vector<Value> initValues,
                               Value at,
                               Value eps) {
        // For some reason, even though there are 6 k-s this is order 4 and 5 method since the last 2 k-s are calculated at the same point
        // (not order 5 and 6 as you might initially think)
        const std::vector<std::vector<Value>> bT({
            {0,     0},
            {0.2,   0.2,            0},
            {0.3,   3.0/40,         9.0/40,         0},
            {0.8,   44.0/45,        -56.0/15,       32.0/9,         0},
            {8.0/9, 19372.0/6561,	-25360.0/2187,	64448.0/6561,	-212.0/729},
            {1.0,   9017.0/3168,	-355.0/33,	    46732.0/5247,	49.0/176,	    -5103.0/18656},
            {1.0,   35.0/384,	    0,	            500.0/1113,	    125.0/192,	    -2187.0/6784,	    11.0/84},
            {0,     35.0/384,	    0,	            500.0/1113,	    125.0/192,	    -2187.0/6784,	    11.0/84,    0},
            {0,     5179.0/57600,	0,              7571.0/16695,	393.0/640,	    -92097.0/339200,	187.0/2100,	0.025}
        });
        return ASRKMasterSolve<Value>(function, initValues, at, eps, bT);
    }

    template<typename Value>
    std::vector<Value> ASRKDormandPrinceSystemSolve(const std::vector<std::shared_ptr<Expression<Value>>>& functions,
                               std::vector<Value> initValues,
                               Value at,
                               Value eps) {
        const std::vector<std::vector<Value>> bT({
            {0,     0},
            {0.2,   0.2,            0},
            {0.3,   3.0/40,         9.0/40,         0},
            {0.8,   44.0/45,        -56.0/15,       32.0/9,         0},
            {8.0/9, 19372.0/6561,	-25360.0/2187,	64448.0/6561,	-212.0/729},
            {1.0,   9017.0/3168,	-355.0/33,	    46732.0/5247,	49.0/176,	    -5103.0/18656},
            {1.0,   35.0/384,	    0,	            500.0/1113,	    125.0/192,	    -2187.0/6784,	    11.0/84},
            {0,     35.0/384,	    0,	            500.0/1113,	    125.0/192,	    -2187.0/6784,	    11.0/84,    0},
            {0,     5179.0/57600,	0,              7571.0/16695,	393.0/640,	    -92097.0/339200,	187.0/2100,	0.025}
        });
        return ASRKMasterSystemSolve<Value>(functions, initValues, at, eps, bT);
    }

}
