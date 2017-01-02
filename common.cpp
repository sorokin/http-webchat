#include "common.h"

const std::string& toLowerCase(const std::string& string) {
    std::string result;
    std::transform(string.begin(), string.end(), result.begin(), ::tolower);
    return result;
}
