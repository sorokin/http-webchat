#ifndef HTTPWEBCHAT_COMMON_H
#define HTTPWEBCHAT_COMMON_H

#include <algorithm>
#include <string>

#include <string.h>

template <typename T>
T _uwv_system_call(T call, T wrong, std::string msg) {
    if (call == wrong) {
        throw msg + " - " + strerror(errno);
    } else {
        return call;
    }
}

template <typename T>
T _m1_system_call(T call, std::string msg) {
    return _uwv_system_call(call, -1, msg);
}

std::string toLowerCase(const std::string& string);

#endif //HTTPWEBCHAT_COMMON_H
