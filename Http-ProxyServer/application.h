#ifndef APPLICATION_H
#define APPLICATION_H
#include <sys/epoll.h>
#include <functional>
#include <map>
#include <vector>
#include <unistd.h>

class Application
{
public:
    typedef std::function <void(epoll_event)> Handler;

    static Application* instance() {
        return mInstance;
    }

    int setHandler(int fd, Handler handler, __uint32_t flags);
    void removeHandler(int fd);
    void changeFlags(int fd, __uint32_t flags);
    int exec();
    ~Application();
private:
    static Application *mInstance;

    Application();
    const int MAX_EVENTS;
    int mainLoopFD;
    std::vector <epoll_event> events;
    std::map <int, Handler> handlers;
};

#endif // APPLICATION_H
