#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <cstring>
#include <string>
#include <vector>
#include <map>

class HttpResponse
{
    typedef std::string String;
    int mStatusCode;
    String mReasonPhrase;
    const char* mBody;
    String mVersion;
    std::map <String, String> mHeaders;
public:
    HttpResponse();
    HttpResponse(int statusCode, const String& reasonPhrase, const char* message = NULL, const String& version = "1.0");
    void setStatusCode(int statusCode);
    int statusCode() const;
    void setReasonPhrase(const String& rph);
    String reasonPhrase() const;
    void setMessageBody(const String& message);
    void setMessageBody(const char* message);
    const char* messageBody() const;
    void setHeader(const String& key, const String& value);
    String header(const String& key);
    std::map<String, String> headers() const;
};

#endif // HTTPRESPONSE_H
