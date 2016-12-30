#include "tcpserversocket.h"

TcpServerSocket::TcpServerSocket(Application *app): MAX_EVENTS(128), listenerFD(NONE), app(app) {}

void TcpServerSocket::listen(const std::string& host, const uint16_t port,
                                                        IncomingConnectionHandler incomingConnectionHandler) {
//                             NewConnectionHandler newConnection) {
    if (listenerFD != NONE)
        throw "The server socket is already listening";

//    listenerFD = socket(AF_INET, SOCK_STREAM, 0);
//    if (listenerFD < 0) {
//        return SocketCreationError;
//    }
//
//    sockaddr_in serv_addr;
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_addr.s_addr = INADDR_ANY;
//    serv_addr.sin_port = htons(port);
//    if (bind(listenerFD, (sockaddr *) &serv_addr, sizeof serv_addr) != 0) {
//        ::close(listenerFD);
//        listenerFD = NONE;
//        return BindError;
//    }
//
//    try {
//        makeSocketNonBlocking(listenerFD);
//    } catch (string error) {
//        ::close(listenerFD);
//        cerr << error << endl;
//        return NonBlockingError;
//    }
//
//    int s = ::listen(listenerFD, SOMAXCONN);
//    if (s != 0) {
//        ::close(listenerFD);
//        return ListenError;
//    }
//
//    app->setHandler(listenerFD, [this](epoll_event ev) {
//        try {
//            acceptConnection(ev, incomingConnectionHandler);
//        } catch (string err) {
//            cerr << "Couldn't accept a connection: " << err << endl;
//        }
//    }, EPOLLIN);
//
//    this->port = port;
//    this->host = host;
////    this->newConnectionHandler = newConnection;
//    return Success;

    listenerFD = _m1_system_call(socket(AF_INET, SOCK_STREAM, 0), "Couldn't create the listening socket");
    try {
        int opt = 1;
        _m1_system_call(setsockopt(listenerFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt),
                        "Couldn't make the listening socket reusable");

        sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = INADDR_ANY;
        sa.sin_port = htons(port);
        _m1_system_call(bind(listenerFD, (sockaddr*) &sa, sizeof sa), "Couldn't bind the listening socket");

        makeSocketNonBlocking(listenerFD);
        _m1_system_call(::listen(listenerFD, SOMAXCONN), "Couldn't execute listen on the listening socket");

        app->setHandler(listenerFD, [=](epoll_event ev) {
            try {
                acceptConnection(ev, incomingConnectionHandler);
            } catch (string error) {
                cerr << "Couldn't accept an incoming connection: " << error << endl;
            }
        }, EPOLLIN);

        this->host = host;
        this->port = port;
    } catch (string error) {
        ::close(listenerFD);
        throw error;
    }
}

//bool TcpServerSocket::isListening() {
//    return listenerFD != NONE;
//}
//
//std::string TcpServerSocket::serverHost() {
//    return host;
//}
//
//unsigned int TcpServerSocket::serverPort() {
//    return port;
//}

//void TcpServerSocket::close() {
//    if (listenerFD == NONE)
//        return;
//    app->removeHandler(listenerFD);
//    int s = ::close(listenerFD);
//    listenerFD = NONE;
//    port = 0;
//    host = "";
//}

TcpServerSocket::~TcpServerSocket() {
    app->removeHandler(listenerFD);
    ::close(listenerFD);
//    pendingConstructorFunctor = PendingConstructorFunctor();
//    newConnectionHandler = NewConnectionHandler();
}

void TcpServerSocket::makeSocketNonBlocking(int socket) {
//    flags = fcntl(socket, F_GETFL, 0);
//    if (flags == -1) {
//        printf("error: makeSocketNonBlocking: fcntl : get flags\n %s", gai_strerror(flags));
//        return NONE;
//    }
    int flags = _m1_system_call(fcntl(socket, F_GETFL, 0), "Couldn't get flags of socket");
//    s = fcntl(socket, F_SETFL, flags  | O_NONBLOCK);
//    if (s != 0) {
//        printf("error: makeSocketNonBlocking: fcntl : set flags\n");
//        return NONE;
//    }
//    return 0;
    _m1_system_call(fcntl(socket, F_SETFL, flags | O_NONBLOCK), "Couldn't make socket non-blocking");
}

bool TcpServerSocket::isErrorSocket(const epoll_event& ev) {
    return (ev.events & EPOLLERR) || (ev.events & EPOLLHUP) || !(ev.events & EPOLLIN);
}

TcpSocket* TcpServerSocket::acceptSocket(const epoll_event& ev, char hbuf[], socklen_t hs, char sbuf[], socklen_t ss) {
    if (isErrorSocket(ev)) {
        return NULL;
    }

    sockaddr in_addr;
    socklen_t in_len = sizeof in_addr;
    int incomingFD = _m1_system_call(accept(listenerFD, &in_addr, &in_len), "Couldn't accept an incoming socket");

    try {
        makeSocketNonBlocking(incomingFD);
        int res = getnameinfo(&in_addr, in_len,
                              hbuf, hs,
                              sbuf, ss, NI_NUMERICHOST | NI_NUMERICSERV);
        if (res != 0) {
            throw string("Couldn't get server info - ") + gai_strerror(res);
        }
        return new TcpSocket(app, incomingFD, hbuf, sbuf);
    } catch (string error) {
        ::close(incomingFD);
        throw error;
    }
}

void TcpServerSocket::acceptConnection(const epoll_event& ev, IncomingConnectionHandler incomingConnectionHandler) {
//    if (isErrorSocket(ev)) {
//        return;
//    }
//
//    sockaddr in_addr;
//    socklen_t in_len = sizeof in_addr;
//    int incomingFD = accept(listenerFD, &in_addr, &in_len);

//    pendingConstructorFunctor = [=]() {
//        makeSocketNonBlocking(incomingFD);
//        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
//        getnameinfo(&in_addr, in_len,
//                        hbuf, sizeof hbuf,
//                        sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
//        return new TcpSocket(app, incomingFD, hbuf, sbuf);
//    };
//    if (newConnectionHandler)
//        newConnectionHandler();
//    if (pendingConstructorFunctor)
//        ::close(incomingFD);
//    pendingConstructorFunctor = PendingConstructorFunctor();

//    try {
//        makeSocketNonBlocking(incomingFD);
//        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
//        getnameinfo(&in_addr, in_len,
//                    hbuf, sizeof hbuf,
//                    sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
//        TcpSocket* incomingConnection = new TcpSocket(app, incomingFD, hbuf, sbuf);
//        incomingConnectionHandler(incomingConnection);
//    } catch (string error) {
//        ::close(incomingFD);
//        throw error;
//    }

    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    incomingConnectionHandler(acceptSocket(ev, hbuf, sizeof hbuf, sbuf, sizeof sbuf));
}

//TcpSocket* TcpServerSocket::getPendingConnection() {
//    if (!pendingConstructorFunctor)
//        return NULL;
//    TcpSocket *ret = pendingConstructorFunctor();
//    pendingConstructorFunctor = PendingConstructorFunctor();
//    return ret;
//}

TcpServerSocket::TcpServerSocket(TcpServerSocket &&oth):
    MAX_EVENTS(oth.MAX_EVENTS), listenerFD(oth.listenerFD), pendingFD(oth.pendingFD), port(oth.port) {
    host.swap(oth.host);
//    pendingConstructorFunctor.swap(oth.pendingConstructorFunctor);
//    newConnectionHandler.swap(oth.newConnectionHandler);
}
