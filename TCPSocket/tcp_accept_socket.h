#ifndef HTTPWEBCHAT_TCPACCEPTSOCKET_H
#define HTTPWEBCHAT_TCPACCEPTSOCKET_H


#include <cinttypes>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "tcp_server_socket.h"

typedef std::function<void(TcpServerSocket*)> AcceptHandler;

class TcpAcceptSocket: public TcpSocket {
    void accept(const epoll_event&, AcceptHandler);
public:
    TcpAcceptSocket(Poller&, const std::string&, uint16_t, AcceptHandler);
};


#endif //HTTPWEBCHAT_TCPSERVERSOCKET_H
