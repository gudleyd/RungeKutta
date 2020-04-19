//
// Created by Ivan on 19.04.2020.
//

#pragma once

#include <string>

namespace utils_rk {

    std::pair<double, bool> stringToDouble(const std::string& s);

    void replace(std::string& s, const std::string& from, const std::string& to);
}