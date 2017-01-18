#ifndef HTTPWEBCHAT_COMMON_H
#define HTTPWEBCHAT_COMMON_H


#include <algorithm>
#include <stdexcept>

#include <string.h>

class OwnException: public std::runtime_error {
public:
    OwnException(const std::string&);
    OwnException(const char*);
};

template <typename T>
inline T _uwv_system_call(T call, T wrong, std::string msg) {
    if (call == wrong) {
        throw OwnException(msg + " - " + strerror(errno));
    } else {
        return call;
    }
}

template <typename T>
inline T _m1_system_call(T call, std::string msg) {
    return _uwv_system_call(call, (T) -1, msg);
}

std::string toLowerCase(const std::string& string);

#endif //HTTPWEBCHAT_COMMON_H
