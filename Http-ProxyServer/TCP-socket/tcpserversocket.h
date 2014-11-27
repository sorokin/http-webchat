#ifndef TCPSERVERSOCKET_H
#define TCPSERVERSOCKET_H
#include <sys/types.h>
#include <sys/epoll.h>
#include <cstring>
#include <string>
#include <functional>
#include <netdb.h>
#include <TCP-socket/tcpsocket.h>
#include <unistd.h>
#include <fcntl.h>
#include <application.h>

class TcpServerSocket
{
public:
    typedef std::function<void()> NewConnectionHandler;
    typedef std::function <TcpSocket*()> PendingConstructorFunctor;
    enum ConnectedState {SuccessConnected, AlreadyBinded, AlreadyConnected, UnknownError};

    TcpServerSocket();
    ConnectedState listen(const std::string& host, const unsigned int port, NewConnectionHandler newConnectionHandler);
    bool isListening();
    std::string serverHost();
    unsigned int serverPort();
    TcpSocket* getPendingConnecntion();
    void close();
    TcpServerSocket(TcpServerSocket&&);
    ~TcpServerSocket();
private:
    static const int NONE = -1;
    const int MAX_EVENTS;
    int listenerFD;
    int pendingFD;
    int port;
    std::string host;
    PendingConstructorFunctor pendingConstructorFunctor;
    NewConnectionHandler newConnectionHandler;

    TcpServerSocket(const TcpServerSocket&) = delete;
    TcpServerSocket& operator = (const TcpServerSocket&) = delete;
    int makeSocketNonBlocking(int listenerFD);
    int addToEpoll(int listenerFD);
    void handler();
    bool isErrorSocket(const epoll_event& ev);
    void acceptConnection(const epoll_event& flags);
};

#endif // TCPSERVERSOCKET_H
