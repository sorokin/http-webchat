#include "common.h"

std::string toLowerCase(const std::string& string) {
    std::string result = string;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

OwnException::OwnException(const std::string& msg): runtime_error(msg) {}

OwnException::OwnException(const char* msg): runtime_error(msg) {}
