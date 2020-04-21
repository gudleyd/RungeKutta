//
// Created by Ivan on 19.04.2020.
//

#include "utils.h"

namespace utils_rk {


    std::pair<float, bool> stringToFloat(const std::string& s) {
        char* p;
        float converted = std::strtof(s.c_str(), &p);
        if (*p)
            return {0, 0};
        else
            return {converted, 1};
    }

    std::pair<double, bool> stringToDouble(const std::string& s) {
        char* p;
        double converted = std::strtod(s.c_str(), &p);
        if (*p)
            return {0, 0};
        else
            return {converted, 1};
    }

    std::pair<long double, bool> stringToLongDouble(const std::string& s) {
        char* p;
        long double converted = std::strtold(s.c_str(), &p);
        if (*p)
            return {0, 0};
        else
            return {converted, 1};
    }

    void replace(std::string& s, const std::string& from, const std::string& to) {

        size_t pos = s.find(from);
        while( pos != std::string::npos)
        {
            s.replace(pos, from.size(), to);
            pos = s.find(from, pos + to.size());
        }
    }

    std::string typeNameToString(const char* mangled)
    {
        int status;
        std::unique_ptr<char[], void (*)(void*)> result(
                abi::__cxa_demangle(mangled, nullptr, nullptr, &status), std::free);
        return result ? std::string(result.get()) : "error occurred";
    }

}
