#ifndef HTTPWEBCHAT_CHATSERVER_H
#define HTTPWEBCHAT_CHATSERVER_H


#include <fstream>

#include "../HTTP/http_server.h"
#include "json.h"

class ChatServer {
public:
    class Message: public JSON {
        std::string from;
        time_t time;
        std::string text;

        static std::map<std::string, JSON> makeFields(const std::string&, time_t, const std::string&);
    public:
        Message(const std::string&, time_t, const std::string&);
    };

    class Object {
        size_t size;
        std::map<std::string, JSON::Type> types;
    public:
        Object(const std::map<std::string, JSON::Type>&);

        std::map<std::string, JSON> match(const std::string&);
    };
private:
    HttpServer httpServer;
    std::vector<Message> history;
    std::map<std::string, size_t> firstMessage, firstUnreadMessage;

    static std::string parseMessage(const std::string&);
    std::string historyAsJson(size_t, size_t);
    static void logError(const HttpRequest&, int, const std::string&);
public:
    ChatServer(uint16_t);
};


#endif //HTTPWEBCHAT_CHATSERVER_H
