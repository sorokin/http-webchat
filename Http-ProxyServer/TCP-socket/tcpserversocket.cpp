#include "tcpserversocket.h"

TcpServerSocket::TcpServerSocket():MAX_EVENTS(128), listenerFD(NONE) {}

bool TcpServerSocket::listen(const std::string& host, const unsigned int port,
                             NewConnectionHandler newConnection) {
    if (listenerFD != NONE)
        return false;
    addrinfo hints;
    addrinfo *result;
    int s;

    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;

    char sport[20];
    sprintf(sport, "%d", port);
    s = getaddrinfo(host.c_str(), sport, &hints, &result);
    if (s != 0 || result == NULL) {
        printf("getaddrinfo: %s\n", gai_strerror(s));
        return false;
    }

    listenerFD = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenerFD == NONE)
        return false;
    if (bind(listenerFD, result->ai_addr, result->ai_addrlen) != 0) {
        listenerFD = NONE;
        return false;
    }
    if (makeSocketNonBlocking(listenerFD) != 0) {
        listenerFD = NONE;
        return false;
    }
    if (::listen(listenerFD, SOMAXCONN) != 0) {
        listenerFD = NONE;
        return false;
    }
    Application::instance()->setHandler(listenerFD , [this](epoll_event ev)
                                                     {acceptConnection(ev);}, EPOLLIN);

    this->port = port;
    this->host = host;
    this->newConnection = newConnection;
    return true;
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
    ::close(listenerFD);
    Application::instance()->removeHandler(listenerFD);
    listenerFD = NONE;
    port = 0;
    host = "";
}

TcpServerSocket::~TcpServerSocket() {
    close();
    //TODO write this
    //remove from Apllication
}

int TcpServerSocket::makeSocketNonBlocking (int socket) {
    int flags, s;
    flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        printf("error: makeSocketNonBlocking: fcntl : get flags\n %s", gai_strerror(flags));
        return NONE;
    }

    flags |= O_NONBLOCK;
    s = fcntl(socket, F_SETFL, flags);
    if (s != 0) {
        printf("error: makeSocketNonBlocking: fcntl : set flags\n");
        return NONE;
    }
    return 0;
}

/*void TcpServerSocket::handler() {//TODO only incoming connections
    int n = epoll_wait(epollFD, events, MAX_EVENTS, -1);
    for (int i = 0; i < n; ++i)
        if (isErrorSocket(events[i]))
            ::close(events[i].data.fd);
        else if (events[i].data.fd == listenerFD)
            acceptConnections();
}*/

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
    int incomingFD = accept4(listenerFD, &in_addr, &in_len, O_NONBLOCK);
    pendingConstructorFunctor = [=]() {
        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
        int s = getnameinfo(&in_addr, in_len,
                        hbuf, sizeof hbuf,
                        sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
        printf("Accepted connection on descriptor %d "
             "(host=%s, port=%s)\n", incomingFD, hbuf, sbuf);
        //makeSocketNonBlocking(incomingFD);
        return new TcpSocket(incomingFD, hbuf, sbuf);
    };
    if (newConnection)
        newConnection();
    //if (pendingConstructorFunctor) ::close(incomingFD);
    pendingConstructorFunctor = PendingConstructorFunctor();
}

TcpSocket* TcpServerSocket::getPendingConnecntion() {
    if (!pendingConstructorFunctor)
        return NULL;
    TcpSocket *ret = pendingConstructorFunctor();
    pendingConstructorFunctor = PendingConstructorFunctor();
    return ret;
}
