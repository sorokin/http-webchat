#ifndef HTTPWEBCHAT_POLLER_H
#define HTTPWEBCHAT_POLLER_H


#include <cstring>
#include <functional>
#include <map>

#include <signal.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "common.h"

class Poller {
public:
    typedef std::function<void(epoll_event)> Handler;

    static Poller& Instance();

    void setHandler(int, Handler, uint32_t);
    void setEvents(int, uint32_t);
    void removeHandler(int);
    void poll();
private:
    static const size_t MAX_EVENTS = 128;

    int efd;
    int sfd;
    epoll_event events[MAX_EVENTS];
    std::map<int, Handler> handlers;

    static Poller instance;

    Poller();
    Poller(const Poller&) = delete;
    ~Poller();
    Poller& operator=(const Poller&) = delete;

    static void signalHandler(int);
};


#endif //HTTPWEBCHAT_POLLER_H
