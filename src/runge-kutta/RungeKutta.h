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
    void RK_solve(std::vector<std::shared_ptr<Expression<Value>>>& functions,
                  const std::vector<std::string>& variables,
                  const std::vector<Value>& initValues,
                  Value h = 0.001,
                  bool mustCompile = true) {
        for (auto& e: functions) {
            if (!e->compile() && mustCompile)
                throw std::runtime_error("Function was not compiled, which can cause performance issues.\n"
                                         "If u don't want to see this error, set mustCompile = false");
        }
    }
}
