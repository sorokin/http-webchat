#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <cstring>
#include <string>
#include <vector>

class HttpResponse
{
public:
    HttpResponse();
    HttpResponse(int statusCode, const std::string reasonPhrase);
    void setStatusCode(int statusCode);
    int statusCode();
    void setReasonPhrase(const std::string& rph);
    std::string reasonPhrase();
    void setMessageBody(const std::string message);
    std::string messageBody();
    std::string header(const std::string& key);
    std::vector <std::pair <std::string, std::string> > headers();
};

#endif // HTTPRESPONSE_H
