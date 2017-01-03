#ifndef HTTPWEBCHAT_TCPSOCKET_H
#define HTTPWEBCHAT_TCPSOCKET_H


#include <fcntl.h>

#include "../poller.h"

class TcpSocket {
protected:
    std::string host;
    uint16_t port;
    int fd;
public:
    static const int NONE;

    TcpSocket(int, const std::string&, uint16_t);
    ~TcpSocket();

    virtual void close();

    bool isOpened() const;
};


#endif //HTTPWEBCHAT_TCPSOCKET_H
