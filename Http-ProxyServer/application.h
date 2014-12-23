#ifndef APPLICATION_H
#define APPLICATION_H
#include <sys/epoll.h>
#include <functional>
#include <map>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <signal.h>

#include <iostream>
using namespace std;

class Application
{
public:
    typedef std::function <void(epoll_event)> Handler;
    typedef std::function <void()> AbortHandler;

    Application();

    int setHandler(int fd, Handler handler, __uint32_t flags);
    void removeHandler(int fd);
    void changeFlags(int fd, __uint32_t flags);
    int exec();
    ~Application();

    static void setAbortHandler(AbortHandler handler) {
        abortHandler = handler;
        signal(SIGINT, Application::sigHandler);
        signal(SIGTERM, Application::sigHandler);
    }
private:
    static void sigHandler(int) {
        if (abortHandler)
            abortHandler();
        exit(0);
    }
    static AbortHandler abortHandler;

    const int MAX_EVENTS;
    int mainLoopFD;
    std::vector <epoll_event> events;
    std::map <int, Handler> handlers;
};

#endif // APPLICATION_H
