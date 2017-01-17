#ifndef HTTPWEBCHAT_POLLER_H
#define HTTPWEBCHAT_POLLER_H


#include <iostream>
#include <functional>
#include <map>

#include <signal.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/signalfd.h>

#include "common.h"

typedef std::function<void(epoll_event)> EventHandler;

class Poller {
    static const size_t MAX_EVENTS = 128;

    static int efd;
    static int sfd;
    static epoll_event events[MAX_EVENTS];
    static std::map<int, EventHandler> handlers;

    static void _start();
    static void _stop();
public:
    static void setHandler(int, const EventHandler&, uint32_t);
    static void setEvents(int, uint32_t);
    static size_t removeHandler(int);

    static void poll();

    Poller();
    ~Poller();
};


#endif //HTTPWEBCHAT_POLLER_H
