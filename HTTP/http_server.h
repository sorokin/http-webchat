#ifndef HTTPWEBCHAT_HTTPSERVER_H
#define HTTPWEBCHAT_HTTPSERVER_H


#include <set>
#include <vector>

#include <time.h>
#include <sys/timerfd.h>

#include "../TCPSocket/tcp_accept_socket.h"
#include "http_request.h"
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
        void end(HttpResponse&);
    };

    typedef std::function<void(const HttpRequest&, ResponseSocket)> RequestHandler;

    static RequestHandler defaultHandler;
private:
    int tfd;
    std::set<TcpServerSocket*> sockets;
    std::vector<std::pair<RouteMatcher, RequestHandler>> matchers;
    std::vector<std::pair<RouteMatcher, RequestHandler>> commonMatchers;

    Poller& poller;
    TcpAcceptSocket listener;

    void processRequest(TcpServerSocket*, const HttpRequest&);
public:
    HttpServer(Poller&, uint16_t);
    ~HttpServer();

    void addRouteMatcher(const RouteMatcher&, const RequestHandler&);
};


#endif //HTTPWEBCHAT_HTTPSERVER_H
