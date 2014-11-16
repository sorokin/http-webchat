#ifndef APPLICATION_H
#define APPLICATION_H
#include <sys/epoll.h>
#include <functional>
#include <map>

class Application
{
public:
    typedef std::function <void(epoll_event)> Handler;

    static Application* instance() {
        return mInstance;
    }

    int setHandler(int fd, Handler handler, __uint32_t flags);
    void removeHandler(int fd);
    int exec();
    ~Application();
private:
    static Application *mInstance;

    Application();
    const int MAX_EVENTS;
    int mainLoopFD;
    epoll_event *events;
    std::map <int, Handler> handlers;
};

#endif // APPLICATION_H
