#ifndef TCPSERVERSOCKET_H
#define TCPSERVERSOCKET_H

#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <netdb.h>
#include <string>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>

#include "../application.h"

#include "tcpsocket.h"

class TcpServerSocket
{
public:
//    typedef std::function<void()> NewConnectionHandler;
    typedef std::function<void(TcpSocket*)> IncomingConnectionHandler;
//    typedef std::function<TcpSocket*()> PendingConstructorFunctor;
    enum ConnectedState {Success, AlreadyConnected, BindError, SocketCreationError, NonBlockingError, ListenError};

    TcpServerSocket(Application *app);
//    ConnectedState listen(const std::string& host, const uint16_t port, NewConnectionHandler newConnectionHandler);
    void listen(const std::string& host, const uint16_t port, IncomingConnectionHandler incomingConnectionHandler);
//    bool isListening();
//    std::string serverHost();
//    unsigned int serverPort();
//    TcpSocket* getPendingConnection();
//    void close();
    TcpServerSocket(TcpServerSocket&&);
    ~TcpServerSocket();
private:
    static const int NONE = -1;
    const int MAX_EVENTS;

    int listenerFD;
    int pendingFD;
    int port;

    Application *app;
    std::string host;
//    PendingConstructorFunctor pendingConstructorFunctor;
//    NewConnectionHandler newConnectionHandler;

    TcpServerSocket(const TcpServerSocket&) = delete;
    TcpServerSocket& operator = (const TcpServerSocket&) = delete;
    void makeSocketNonBlocking(int listenerFD);
//    int addToEpoll(int listenerFD);
//    void handler();
    static bool isErrorSocket(const epoll_event& ev);
    TcpSocket* acceptSocket(const epoll_event& ev, char hbuf[], socklen_t hs, char sbuf[], socklen_t ss);
    void acceptConnection(const epoll_event& flags, IncomingConnectionHandler incomingConnectionHandler);
};

#endif // TCPSERVERSOCKET_H
