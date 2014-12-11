#include "tcpserversocket.h"
#include <cassert>

TcpServerSocket::TcpServerSocket(Application *app):MAX_EVENTS(128), listenerFD(NONE), app(app) {}

TcpServerSocket::ConnectedState TcpServerSocket::listen(const std::string& host, const unsigned int port,
                             NewConnectionHandler newConnection) {
    if (listenerFD != NONE)
        return AlreadyConnected;
    listenerFD = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenerFD >= 0);

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(listenerFD, (sockaddr*)&serv_addr, sizeof serv_addr) != 0) {
        listenerFD = NONE;
        return AlreadyBinded;
    }

    int s = makeSocketNonBlocking(listenerFD);
    assert(s == 0);

    s = ::listen(listenerFD, SOMAXCONN);
    assert(s == 0);

   app->setHandler(listenerFD , [this](epoll_event ev)
                                                     {acceptConnection(ev);}, EPOLLIN);

    this->port = port;
    this->host = host;
    this->newConnectionHandler = newConnection;
    return Success;
}

bool TcpServerSocket::isListening() {
    return listenerFD != NONE;
}

std::string TcpServerSocket::serverHost() {
    return host;
}

unsigned int TcpServerSocket::serverPort() {
    return port;
}

void TcpServerSocket::close() {
    if (listenerFD == NONE)
        return;
    int s = ::close(listenerFD);
    assert(s == 0);
    app->removeHandler(listenerFD);
    listenerFD = NONE;
    port = 0;
    host = "";
}

TcpServerSocket::~TcpServerSocket() {
    close();
    pendingConstructorFunctor = PendingConstructorFunctor();
    newConnectionHandler = NewConnectionHandler();
}

int TcpServerSocket::makeSocketNonBlocking (int socket) {
    int flags, s;
    flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        printf("error: makeSocketNonBlocking: fcntl : get flags\n %s", gai_strerror(flags));
        return NONE;
    }
    s = fcntl(socket, F_SETFL, flags  | O_NONBLOCK);
    if (s != 0) {
        printf("error: makeSocketNonBlocking: fcntl : set flags\n");
        return NONE;
    }
    return 0;
}

bool TcpServerSocket::isErrorSocket(const epoll_event& ev) {
    return (ev.events & EPOLLERR) || (ev.events & EPOLLHUP) || !(ev.events & EPOLLIN);
}

void TcpServerSocket::acceptConnection(const epoll_event& ev) {
    if (isErrorSocket(ev)) {
        close();
        return;
    }

    sockaddr in_addr;
    socklen_t in_len = sizeof in_addr;
    int incomingFD = accept(listenerFD, &in_addr, &in_len);
    pendingConstructorFunctor = [=]() {
        makeSocketNonBlocking(incomingFD);
        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
        int s = getnameinfo(&in_addr, in_len,
                        hbuf, sizeof hbuf,
                        sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
        printf("Accepted connection on descriptor %d "
             "(host=%s, port=%s)\n", incomingFD, hbuf, sbuf);
        //makeSocketNonBlocking(incomingFD);
        return new TcpSocket(app, incomingFD, hbuf, sbuf);
    };
    if (newConnectionHandler)
        newConnectionHandler();
    if (pendingConstructorFunctor) ::close(incomingFD);//TODO good?
    pendingConstructorFunctor = PendingConstructorFunctor();
}

TcpSocket* TcpServerSocket::getPendingConnecntion() {
    if (!pendingConstructorFunctor)
        return NULL;
    TcpSocket *ret = pendingConstructorFunctor();
    pendingConstructorFunctor = PendingConstructorFunctor();
    return ret;
}

TcpServerSocket::TcpServerSocket(TcpServerSocket &&oth):
    MAX_EVENTS(oth.MAX_EVENTS), listenerFD(oth.listenerFD), pendingFD(oth.pendingFD), port(oth.port) {
    host.swap(oth.host);
    pendingConstructorFunctor.swap(oth.pendingConstructorFunctor);
    newConnectionHandler.swap(oth.newConnectionHandler);
}
