//
// Created by gaeqs on 4/11/24.
//

#include "StringUtils.h"

namespace neon {
    std::string replace(const std::string& str, const std::string& toReplace, const std::string& replacement) {
        std::size_t pos = str.find(toReplace);
        if (pos == std::string::npos) return str;
        std::string result = str;
        result.replace(pos, toReplace.length(), replacement);
        return result;
    }
}