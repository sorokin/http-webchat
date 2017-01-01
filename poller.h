#ifndef HTTPWEBCHAT_POLLER_H
#define HTTPWEBCHAT_POLLER_H


#include <cstring>
#include <iostream>
#include <functional>
#include <map>

#include <signal.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>

#include "common.h"

typedef std::function<void(epoll_event)> EventHandler;

class Poller {
    static const size_t MAX_EVENTS = 128;

    int efd;
    int sfd;
    epoll_event events[MAX_EVENTS];
    std::map<int, EventHandler> handlers;

    static Poller instance;

    Poller();
    Poller(const Poller&) = delete;
    ~Poller();
    Poller& operator=(const Poller&) = delete;

    static void signalHandler(int);
public:
    static Poller& Instance();

    void setHandler(int, EventHandler, uint32_t);
    void setEvents(int, uint32_t);
    size_t removeHandler(int);
    void poll();
};


#endif //HTTPWEBCHAT_POLLER_H
