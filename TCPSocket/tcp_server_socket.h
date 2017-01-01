#ifndef HTTPWEBCHAT_TCPSERVERSOCKET_H
#define HTTPWEBCHAT_TCPSERVERSOCKET_H


#include <algorithm>
#include <deque>

#include "tcp_socket.h"

typedef std::function<void(std::deque<char>&)> SocketReceivedDataHandler;
typedef std::function<void()> SocketClosedHandler;

class TcpServerSocket: public TcpSocket {
    std::deque<char> inBuffer;
    std::deque<char> outBuffer;
    SocketReceivedDataHandler receivedDataHandler;
    SocketClosedHandler closedHandler;

    void eventHandler(const epoll_event&);
    virtual void close();
public:
    static const size_t READ_BUFFER_SIZE;
    static const size_t WRITE_BUFFER_SIZE;

    TcpServerSocket(Poller&, int, const std::string&, uint16_t);

    void setReceivedDataHandler(SocketReceivedDataHandler);
    void setClosedHandler(SocketClosedHandler);
    void write(const std::string&);
};


#endif //HTTPWEBCHAT_TCPSERVERSOCKET_H
