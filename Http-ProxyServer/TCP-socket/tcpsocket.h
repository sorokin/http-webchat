#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <cassert>
#include <cstring>
#include <deque>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../application.h"

class TcpSocket {
public:
    typedef std::string Bytes;
    typedef std::function<void()> ClosedConnectionHandler;
    typedef std::function<void()> DataReceivedHandler;
//    enum ConnectedState {SuccessConnected, AlreadyConnected, UnknownHost, UnknownError};

    TcpSocket(Application *app);
//    ConnectedState connectToHost(const std::string& host, unsigned int port = 80);
//    std::string serverHost();
//    unsigned int serverPort();
//    bool write(const char* data, int len);
    bool write(const std::string& s);
    Bytes readBytes();
//    std::string readString();

    void setClosedConnectionHandler(ClosedConnectionHandler);
//    void removeClosedConnectionHandler();
    void setDataReceivedHandler(DataReceivedHandler);
//    void removeDataReceivedHandler();
    void clearBuffers();
//    int bytesAvailable();
    void close();
    TcpSocket(TcpSocket&&);
    bool inCallback;
    bool pendingDelete;
    static void operator delete(void* ptr, size_t size) throw();//DANGEROUS
    ~TcpSocket();
private:
    const uint32_t DEFAULT_FLAGS = EPOLLIN | EPOLLHUP;
    const uint32_t OUT_FLAGS = EPOLLOUT | EPOLLIN | EPOLLHUP;
    const int NONE = -1;
    const size_t BUFFER_SIZE_ON_READ;
    const size_t BUFFER_SIZE_ON_WRITE;

    uint32_t currentFlags;
    int fd;
    unsigned int port;

    Application *app;
    std::string host;
    std::deque <char> readBuffer;
    std::deque <char> writeBuffer;
    ClosedConnectionHandler closedConnectionHandler;
    DataReceivedHandler dataReceivedHandler;

    friend class TcpServerSocket;
    TcpSocket(Application *app, int fd, const char* host, const char* port);
//    int makeSocketNonBlocking(int fd);
    void handler(const epoll_event&);
//    bool isErrorSocket(const epoll_event& ev);
    void appendDataForWrite(const char* data, size_t len);
    void appendCharForWrite(char c);
    void tryWrite();
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator = (const TcpSocket&) = delete;
};

#endif // TCPSOCKET_H
