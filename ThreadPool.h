#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H

#include <condition_variable>
#include <queue>
#include <thread>
#include "HttpConnection.h"

class ThreadPool {

public:

    explicit ThreadPool(const unsigned short &numWorkers);

    virtual ~ThreadPool();

    void addTask(HttpConnection *connection);

    void quitLoop();

    void run();

    static void *work(void *pool);

private:
    std::condition_variable condition;
    std::mutex mutex;
    std::queue<HttpConnection *> tasks;
    bool stop;

};

#endif // WEBSERVER_THREADPOOL_H
