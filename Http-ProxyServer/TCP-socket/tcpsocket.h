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

class TcpSocket
{
public:
    typedef std::function<void()> ClosedConnectionHandler;
    typedef std::function<void()> DataReceivedHandler;

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
    void clearBuffer();
    int bytesAvailable();
    void close();
private:
    TcpSocket(int fd, char* host, char* port);
    static const int NONE = -1;
    int fd;
    const int BUFFER_SIZE;
    friend class TcpServerSocket;
    std::string host;
    unsigned int port;
    std::queue <char> buffer;

    void handler(epoll_event);
    ClosedConnectionHandler closedConnectionHandler;
    DataReceivedHandler dataReceivedHandler;
    bool isErrorSocket(const epoll_event& ev);
};

#endif // TCPSOCKET_H
