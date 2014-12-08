#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H
#include <cstring>
#include <string>
#include <vector>

class HttpRequest
{
public:
    enum Method {HEAD, GET, PUT, POST, DELETE};
    HttpRequest();
    HttpRequest(Method method, const std::string& url, int version = 1);
    void setMethod(const std::string& method);
    std::string method();
    void setUrl(const std::string& url);
    std::string url();
    void setVersion(int vers);
    int version();
    HttpRequest(const std::string& method, const std::string& url, int version = 1);
    void setHeader(const std::string& key, const std::string val);
    void setHeaders(const std::vector <std::pair <std::string, std::string> >& headers);
    void header(const std::string& key);
    void setMessageBody(const std::string message);
    std::string messageBody();

};

#endif // HTTPREQUEST_H
