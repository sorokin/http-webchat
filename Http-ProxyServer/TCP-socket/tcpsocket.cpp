#include "tcpsocket.h"

TcpSocket::TcpSocket():BUFFER_SIZE(1024), fd(NONE) {}

TcpSocket::TcpSocket(int fd, char* host, char* port):TcpSocket() {
    this->fd = fd;
    this->host = std::string(host);
    sscanf(port, "%d", &this->port);
    Application::instance()->setHandler(fd, [this](epoll_event ev)
                                            {handler(ev);}, EPOLLIN | EPOLLOUT | EPOLLET);
}

bool TcpSocket::connectToHost(const std::string& host, unsigned int port) {
    if (fd != NONE)
        return false;
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    hostent *server = gethostbyname(host.c_str());
    bcopy((char *)server->h_addr, (char *)&servAddr.sin_addr.s_addr, server->h_length);
    servAddr.sin_port = htons(port);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(fd, (struct sockaddr *) &servAddr,sizeof(servAddr)) < 0) {
        fd = NONE;
        return false;
    }
    this->host = host;
    this->port = port;
    return true;
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
    ::write(fd, data, len);
    return true;
}

bool TcpSocket::write(const std::string& s) {
    if (fd == NONE)
        return false;
    ::write(fd, s.c_str(), s.size());
    return true;
}

int TcpSocket::readBytes(char *outData) {
    int size = buffer.size();
    outData = new char[buffer.size()];
    for (int i = 0; i < buffer.size(); ++i) {
        outData[i] = buffer.front();
        buffer.pop();
    }
    return size;
}

char* TcpSocket::readBytes() {
    char *outData = new char[buffer.size()];
    for (int i = 0; !buffer.empty(); ++i) {
        outData[i] = buffer.front();
        buffer.pop();
    }
    return outData;
}

char* TcpSocket::readString() {
    char *outData = new char[buffer.size() + 1];
    outData[buffer.size()] = 0;
    for (int i = 0; !buffer.empty(); ++i) {
        outData[i] = buffer.front();
        buffer.pop();
    }
    return outData;
}

void TcpSocket::setCloseConnectionHandler(ClosedConnectionHandler h) {
    closedConnectionHandler = h;
}

void TcpSocket::removeCloseConnectionHandler() {
    closedConnectionHandler = ClosedConnectionHandler();
}

void TcpSocket::setDataReceivedHandler(DataReceivedHandler h) {
    dataReceivedHandler = h;
}

void TcpSocket::removeDataReceivedHandler() {
    dataReceivedHandler = DataReceivedHandler();
}

void TcpSocket::clearBuffer() {
    while (!buffer.empty()) buffer.pop();
}

int TcpSocket::bytesAvailable() {
    return buffer.size();
}

void TcpSocket::close() {
    printf ("Closed connection on descriptor %d\n", fd);
    clearBuffer();
    ::close(fd);
    Application::instance()->removeHandler(fd);
    fd = NONE;
    host = "";
    port = 0;
}

void TcpSocket::handler(epoll_event event) {
    bool done = false;
    bool  empty = true;
    for(;;) {
        char buf[BUFFER_SIZE];
        ssize_t count = ::read(fd, buf, sizeof buf);
        if (count == -1) {
            if (errno != EAGAIN) {
                done = true;
                //read error
            }
            break;
        } else if (count == 0) {
            done = true;
            break;
        } else {
            empty = false;
            for (int i = 0; i < count; ++i)
                buffer.push(buf[i]);
        }
    }

    if (!empty && dataReceivedHandler)
        dataReceivedHandler();

    if (done) {
        close();
        if (closedConnectionHandler)
            closedConnectionHandler();
    }
}

bool TcpSocket::isErrorSocket(const epoll_event& ev) {
    return (ev.events & EPOLLERR) || (ev.events & EPOLLHUP) || !(ev.events & EPOLLIN);
}
