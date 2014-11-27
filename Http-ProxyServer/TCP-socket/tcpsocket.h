#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include <cstring>
#include <sys/epoll.h>
#include <application.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <deque>

class TcpSocket {
public:
    typedef std::function<void()> ClosedConnectionHandler;
    typedef std::function<void()> DataReceivedHandler;
    enum ConnectedState {SuccessConnected, AlreadyConnected, UnknownError};

    TcpSocket();
    ConnectedState connectToHost(const std::string& host, unsigned int port);
    std::string serverHost();
    unsigned int serverPort();
    bool write(const char* data, int len);
    bool write(const std::string& s);
    int readBytes(char *outData);
    char* readBytes();
    char* readString();

    void setCloseConnectionHandler(ClosedConnectionHandler);
    void removeCloseConnectionHandler();
    void setDataReceivedHandler(DataReceivedHandler);
    void removeDataReceivedHandler();
    void clearBuffers();
    int bytesAvailable();
    void close();
    TcpSocket(TcpSocket&&);
    ~TcpSocket();
private:
    const int DEFAULT_FLAGS = EPOLLIN | EPOLLET | EPOLLHUP;
    const int OUT_FLAGS = EPOLLOUT | EPOLLIN | EPOLLET | EPOLLHUP;
    const int NONE = -1;
    const int BUFFER_SIZE_ON_READ;
    const int BUFFER_SIZE_ON_WRITE;
    int fd;
    std::string host;
    unsigned int port;
    std::deque <char> readBuffer;
    std::deque <char> writeBuffer;
    ClosedConnectionHandler closedConnectionHandler;
    DataReceivedHandler dataReceivedHandler;

    friend class TcpServerSocket;
    TcpSocket(int fd, char* host, char* port);
    int makeSocketNonBlocking(int fd);
    void handler(const epoll_event&);
    bool isErrorSocket(const epoll_event& ev);
    void appendDataForWrite(const char* data, int len);
    void appendCharForWrite(char c);
    void tryWrite();
    TcpSocket(const TcpSocket&) = delete;
    TcpSocket& operator = (const TcpSocket&) = delete;
};

#endif // TCPSOCKET_H
