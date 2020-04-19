//
// Created by Ivan on 19.04.2020.
//

#include "utils.h"

namespace utils_rk {

    void replace(std::string& s, const std::string& from, const std::string& to) {

        size_t pos = s.find(from);
        while( pos != std::string::npos)
        {
            s.replace(pos, from.size(), to);
            pos = s.find(from, pos + to.size());
        }
    }
}
