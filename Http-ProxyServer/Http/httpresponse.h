#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <cstring>
#include <string>
#include <vector>
#include <map>

class HttpResponse
{
public:
    typedef std::string String;
    typedef std::string Data;

    HttpResponse();
    HttpResponse(int statusCode, const String& reasonPhrase,
                 const String& version, const Data& message = "");
    void setStatusCode(int statusCode);
    int statusCode() const;
    void setReasonPhrase(const String& rph);
    String reasonPhrase() const;
    void setMessageBody(const Data& message);
    Data messageBody() const;
    void setHeader(const String& key, const String& value);
    String header(const String& key);
    std::map<String, String> headers() const;
private:
    int mStatusCode;
    String mReasonPhrase;
    Data mBody;
    String mVersion;
    std::map <String, String> mHeaders;
};

#endif // HTTPRESPONSE_H
