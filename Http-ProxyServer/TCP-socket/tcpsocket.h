#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include <cstring>
#include <sys/epoll.h>
#include <queue>
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
    bool connectToHost(const std::string& host, unsigned int port);
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
    ~TcpSocket();
private:
    const int DEFAULT_FLAGS = EPOLLIN | EPOLLET;
    const int OUT_FLAGS = EPOLLOUT | EPOLLIN | EPOLLET;
    TcpSocket(int fd, char* host, char* port);
    static const int NONE = -1;
    int fd;
    const int BUFFER_SIZE_ON_READ;
    const int BUFFER_SIZE_ON_WRITE;
    friend class TcpServerSocket;
    std::string host;
    unsigned int port;
    std::queue <char> readBuffer;
    std::deque <char> writeBuffer;
    int makeSocketNonBlocking(int fd);

    void handler(const epoll_event&);
    ClosedConnectionHandler closedConnectionHandler;
    DataReceivedHandler dataReceivedHandler;
    bool isErrorSocket(const epoll_event& ev);
    void appendDataForWrite(const char* data, int len);
    void appendCharForWrite(char c);
    void tryWrite();
};

#endif // TCPSOCKET_H
