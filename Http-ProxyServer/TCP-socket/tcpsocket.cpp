#include "tcpsocket.h"
#include <cassert>

#include <iostream>
using namespace std;

int TcpSocket::makeSocketNonBlocking(int socket) {
    int flags, s;
    flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        printf("error: makeSocketNonBlocking: fcntl : get flags : %s", gai_strerror(flags));
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

TcpSocket::TcpSocket(Application *app):
    fd(NONE), BUFFER_SIZE_ON_READ(1024), BUFFER_SIZE_ON_WRITE(1024),
    inHandler(false), pendingDelete(false), app(app) {
}

TcpSocket::TcpSocket(Application *app, int fd, char* host, char* port):TcpSocket(app) {
    this->fd = fd;
    this->host = std::string(host);
    sscanf(port, "%d", &this->port);
    app->setHandler(fd, [this](const epoll_event& ev)
                                            {handler(ev);}, DEFAULT_FLAGS);
}

TcpSocket::ConnectedState TcpSocket::connectToHost(const std::string& host, unsigned int port) {
    if (fd != NONE)
        return AlreadyConnected;
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    hostent *server = gethostbyname(host.c_str());
    if (server == NULL)
        return UnknownHost;
    bcopy((char *)server->h_addr, (char *)&servAddr.sin_addr.s_addr, server->h_length);
    servAddr.sin_port = htons(port);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    int s = connect(fd, (struct sockaddr *) &servAddr,sizeof(servAddr));
    assert(s == 0);
    s = makeSocketNonBlocking(fd);
    assert(s == 0);
    this->host = host;
    this->port = port;
    app->setHandler(fd, [this](const epoll_event& ev)
                                            {handler(ev);}, DEFAULT_FLAGS);
    return SuccessConnected;
}

std::string TcpSocket::serverHost() {
    return host;
}

unsigned int TcpSocket::serverPort() {
    return port;
}

bool TcpSocket::write(const char* data, int len) {
    if (fd == NONE)
        return false;
    appendDataForWrite(data, len);
    tryWrite();
    return true;
}

bool TcpSocket::write(const std::string& s) {
    if (fd == NONE)
        return false;
    for (int i = 0; i < s.size(); ++i)
        appendCharForWrite(s[i]);
    appendCharForWrite(0);
    tryWrite();
    return true;
}

TcpSocket::Bytes TcpSocket::readBytes() {
    Bytes ret;
    for (int i = 0; !readBuffer.empty(); ++i) {
        ret += readBuffer.front();
        readBuffer.pop_front();
    }
    return ret;
    /*char *outData = new char[readBuffer.size()];
    for (int i = 0; !readBuffer.empty(); ++i) {
        outData[i] = readBuffer.front();
        readBuffer.pop_front();
    }
    return outData;*/
}

std::string TcpSocket::readString() {
    std::string ret;
    for (int i = 0; !readBuffer.empty() && readBuffer.front() != 0; ++i) {
        ret += readBuffer.front();
        readBuffer.pop_front();
    }
    if (!readBuffer.empty())
        readBuffer.pop_back();
    return ret;
}

void TcpSocket::setClosedConnectionHandler(ClosedConnectionHandler h) {
    closedConnectionHandler = h;
}

void TcpSocket::removeClosedConnectionHandler() {
    closedConnectionHandler = ClosedConnectionHandler();
}

void TcpSocket::setDataReceivedHandler(DataReceivedHandler h) {
    dataReceivedHandler = h;
    if (h && !readBuffer.empty())
        h();
}

void TcpSocket::removeDataReceivedHandler() {
    dataReceivedHandler = DataReceivedHandler();
}

void TcpSocket::clearBuffers() {
    readBuffer.clear();
    writeBuffer.clear();
}

int TcpSocket::bytesAvailable() {
    return readBuffer.size();
}

void TcpSocket::close() {
    if (fd == NONE)
        return;
    //printf("Closed connection on descriptor %d\n", fd);
    clearBuffers();
    int s = ::close(fd);
    assert(s == 0);
    app->removeHandler(fd);
    fd = NONE;
    host = "";
    port = 0;
}

void TcpSocket::handler(const epoll_event& event) {
    //cerr << "this = " << this << " EPOLLHUP = " << (event.events & EPOLLHUP) << endl;
    if ((event.events & EPOLLHUP) && closedConnectionHandler) {
        inHandler = true;
        closedConnectionHandler();
        inHandler = false;
    }

    if (!pendingDelete && (event.events & EPOLLOUT))
        tryWrite();

    if (!pendingDelete && (event.events & EPOLLIN)) {
        bool done = false;
        bool  empty = true;
        for(;;) {
            char buf[BUFFER_SIZE_ON_READ];
            ssize_t count = ::read(fd, buf, BUFFER_SIZE_ON_READ);
            //cerr << "this = " << this << " count = " << count << endl;
            if (count == -1) {
                if (errno != EAGAIN)
                    done = true;
                break;
            } else if (count == 0) {
                done = true;
                break;
            } else {
                empty = false;
                for (int i = 0; i < count; ++i)
                    readBuffer.push_back(buf[i]);
            }
        }
        //cerr << "size = " << readBuffer.size() << endl;
        if (!empty && dataReceivedHandler && !pendingDelete) {
            inHandler = true;
            dataReceivedHandler();
            inHandler = false;
        }

        if (done) {
            if (closedConnectionHandler && !pendingDelete) {
                inHandler = true;
                closedConnectionHandler();
                inHandler = false;
            }
            close();
        }
    }

    if (pendingDelete)
        ::operator delete(this);
    //cerr << "size = " << readBuffer.size() << endl;
}

void TcpSocket::appendDataForWrite(const char* data, int len) {
    for (int i = 0; i < len; ++i)
        appendCharForWrite(data[i]);
}

void TcpSocket::appendCharForWrite(char c) {
    writeBuffer.push_back(c);
}

void TcpSocket::tryWrite() {
    static int currentFlags = DEFAULT_FLAGS;
    char tmpBuffer[BUFFER_SIZE_ON_WRITE];
    while (!writeBuffer.empty()) {
        int tmpBufferSize = 0;
        std::deque <char>::iterator it = writeBuffer.begin();
        for (;tmpBufferSize < BUFFER_SIZE_ON_WRITE && it != writeBuffer.end(); ++tmpBufferSize, it++)
            tmpBuffer[tmpBufferSize] = *it;
        int len = ::write(fd, tmpBuffer, tmpBufferSize);
        if (len == -1)
            break;
        for (int i = 0; i < len; ++i)
            writeBuffer.pop_front();
    }
    if (writeBuffer.empty() && currentFlags != DEFAULT_FLAGS) {
        app->changeFlags(fd, DEFAULT_FLAGS);
        currentFlags = DEFAULT_FLAGS;
    } else if (currentFlags != OUT_FLAGS) {
        app->changeFlags(fd, OUT_FLAGS);
        currentFlags = OUT_FLAGS;
    }
}

bool TcpSocket::isErrorSocket(const epoll_event& ev) {
    return (ev.events & EPOLLERR) || (ev.events & EPOLLHUP) || !(ev.events & EPOLLIN) || !(ev.events & EPOLLOUT);
}

TcpSocket::~TcpSocket() {
    close();
    closedConnectionHandler = ClosedConnectionHandler();
    dataReceivedHandler = DataReceivedHandler();
}

TcpSocket::TcpSocket(TcpSocket&& oth):BUFFER_SIZE_ON_READ(oth.BUFFER_SIZE_ON_READ),
    BUFFER_SIZE_ON_WRITE(oth.BUFFER_SIZE_ON_WRITE), fd(oth.fd), port(oth.port) {
    host.swap(oth.host);
    writeBuffer.swap(oth.writeBuffer);
    readBuffer.swap(oth.readBuffer);
    closedConnectionHandler.swap(oth.closedConnectionHandler);
    dataReceivedHandler.swap(oth.dataReceivedHandler);
}

static void TcpSocket::operator delete(void* ptr, size_t size) throw() {
    assert(ptr != NULL);
    assert(size == sizeof(TcpSocket));
    TcpSocket *obj = (TcpSocket*)ptr;
    if (!obj->inHandler)
        ::operator delete(ptr);
    else
        obj->pendingDelete = true;

}
