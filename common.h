#ifndef HTTP_PROXYSERVER_COMMON_H
#define HTTP_PROXYSERVER_COMMON_H

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

#endif //HTTP_PROXYSERVER_COMMON_H
