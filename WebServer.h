#ifndef WEBSERVER_WEBSERVER_H
#define WEBSERVER_WEBSERVER_H

#include <netinet/in.h>
#include <csignal>
#include "Utility.h"
#include "ThreadPool.h"
#include "TimeList.h"

class WebServer {

public:

    WebServer(const uint16_t &port, const unsigned short &numWorkers, std::string webRoot);

    virtual ~WebServer();

    void eventLoop();

    void handleCloseOrError(const int &fd);

    void handleComingConnection();

    void handleInput(const int &fd);

    void handleOutput(const int &fd);

private:
    int epollFd;
    epoll_event events[Config::maxEpollWaitEvent];
    std::vector<HttpConnection> httpConnections;
    int listenFd;
    uint16_t port;
    ThreadPool threadPool;
    std::string webRoot;
    int pipeFd[2];

    void handleSignal();
};

#endif // WEBSERVER_WEBSERVER_H
