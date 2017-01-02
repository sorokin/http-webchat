#ifndef HTTPWEBCHAT_HTTPCOMMON_H
#define HTTPWEBCHAT_HTTPCOMMON_H


#include "../common.h"

namespace Http {
    enum Method {GET, HEAD, POST};

    const std::string version1_0 = "HTTP/1.0";
    const std::string version1_1 = "HTTP/1.1";

    std::string methodToString(Method);
    Method stringToMethod(const std::string&);
};


#endif //HTTPWEBCHAT_HTTPCOMMON_H
