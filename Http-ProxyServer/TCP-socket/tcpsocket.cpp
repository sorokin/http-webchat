#include "tcpsocket.h"

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

TcpSocket::TcpSocket():
    fd(NONE), BUFFER_SIZE_ON_READ(1024), BUFFER_SIZE_ON_WRITE(1024) {
    //makeSocketNonBlocking(fd);
}

TcpSocket::TcpSocket(int fd, char* host, char* port):TcpSocket() {
    this->fd = fd;
    this->host = std::string(host);
    sscanf(port, "%d", &this->port);
    Application::instance()->setHandler(fd, [this](const epoll_event& ev)
                                            {handler(ev);}, DEFAULT_FLAGS);
}

bool TcpSocket::connectToHost(const std::string& host, unsigned int port) {
    //printf("connect to host\n");
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
    makeSocketNonBlocking(fd);
    this->host = host;
    this->port = port;
    //printf("apl:instance\n");
    Application::instance()->setHandler(fd, [this](const epoll_event& ev)
                                            {handler(ev);}, DEFAULT_FLAGS);
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
    appendDataForWrite(data, len);
    tryWrite();
    return true;
}

bool TcpSocket::write(const std::string& s) {
    if (fd == NONE)
        return false;
    for (int i = 0; i < s.size(); ++i)
        appendCharForWrite(s[i]);
    tryWrite();
    return true;
}

int TcpSocket::readBytes(char *outData) {
    int size = readBuffer.size();
    outData = new char[readBuffer.size()];
    for (int i = 0; i < readBuffer.size(); ++i) {
        outData[i] = readBuffer.front();
        readBuffer.pop();
    }
    return size;
}

char* TcpSocket::readBytes() {
    char *outData = new char[readBuffer.size()];
    for (int i = 0; !readBuffer.empty(); ++i) {
        outData[i] = readBuffer.front();
        readBuffer.pop();
    }
    return outData;
}

char* TcpSocket::readString() {
    char *outData = new char[readBuffer.size() + 1];
    outData[readBuffer.size()] = 0;
    for (int i = 0; !readBuffer.empty(); ++i) {
        outData[i] = readBuffer.front();
        readBuffer.pop();
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

void TcpSocket::clearBuffers() {
    while (!readBuffer.empty()) readBuffer.pop();
    while (!writeBuffer.empty()) writeBuffer.pop_front();
}

int TcpSocket::bytesAvailable() {
    return readBuffer.size();
}

void TcpSocket::close() {
    //printf("Closed connection on descriptor %d\n", fd);
    clearBuffers();
    ::close(fd);
    Application::instance()->removeHandler(fd);
    fd = NONE;
    host = "";
    port = 0;
}

void TcpSocket::handler(const epoll_event& event) {
    //printf("in handler\n");
    //if ((event.events & EPOLLRDHUP) && closedConnectionHandler)
        //closedConnectionHandler();
    if (event.events & EPOLLOUT)
        tryWrite();
    //printf("handler = %d\n", (event.events & EPOLLIN));
    if (event.events & EPOLLIN) {
        bool done = false;
        bool  empty = true;
        for(;;) {
            char buf[BUFFER_SIZE_ON_READ];
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
                    readBuffer.push(buf[i]);
            }
        }
        //printf("end read. q.size() = %d\n", readBuffer.size());
        if (!empty && dataReceivedHandler)
            dataReceivedHandler();

        if (done) {
            if (closedConnectionHandler)
                closedConnectionHandler();
            close();
        }
    }
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
        Application::instance()->changeFlags(fd, DEFAULT_FLAGS);
        currentFlags = DEFAULT_FLAGS;
    } else if (currentFlags != OUT_FLAGS) {
        Application::instance()->changeFlags(fd, OUT_FLAGS);
        currentFlags = OUT_FLAGS;
    }
}

bool TcpSocket::isErrorSocket(const epoll_event& ev) {
    return (ev.events & EPOLLERR) || (ev.events & EPOLLHUP) || !(ev.events & EPOLLIN) || !(ev.events & EPOLLOUT);
}

TcpSocket::~TcpSocket() {
    close();
}
