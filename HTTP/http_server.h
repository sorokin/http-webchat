#ifndef HTTPWEBCHAT_HTTPSERVER_H
#define HTTPWEBCHAT_HTTPSERVER_H


#include <set>
#include <vector>

#include <sys/timerfd.h>

#include "../TCPSocket/tcp_accept_socket.h"
#include "http_response.h"
#include "route_matcher.h"

class HttpServer {
public:
    class ResponseSocket {
        friend class HttpServer;

        bool valid;
        TcpServerSocket& socket;

        ResponseSocket(TcpServerSocket&);
    public:
        void close();
        void end(HttpResponse&);
    };

    typedef std::function<void(const HttpRequest&, ResponseSocket)> RequestHandler;

    static RequestHandler defaultHandler;
private:
    int tfd;
    std::set<TcpServerSocket*> sockets;
    std::vector<std::pair<RouteMatcher, RequestHandler>> matchers;
    std::vector<std::pair<RouteMatcher, RequestHandler>> commonMatchers;

    TcpAcceptSocket listener;
    Poller& poller;

    void processRequest(TcpServerSocket*, const HttpRequest&);
public:
    HttpServer(uint16_t, Poller&);
    ~HttpServer();

    void addRouteMatcher(const RouteMatcher&, const RequestHandler&);
};


#endif //HTTPWEBCHAT_HTTPSERVER_H
