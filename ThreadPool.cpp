#include "ThreadPool.h"

ThreadPool::ThreadPool(const unsigned short &numWorkers) : stop(false) {
    for (unsigned short i = 0; i < numWorkers; ++i) {
        std::thread thread(ThreadPool::work, this);
        thread.detach();
    }
}

ThreadPool::~ThreadPool() = default;

void ThreadPool::addTask(HttpConnection *connection) {
    {
        std::unique_lock<std::mutex> locker(mutex);
        tasks.push(connection);
    }
    condition.notify_one();
}

/**
 * 停止并退出线程循环
 */
void ThreadPool::quitLoop() {
    {
        std::unique_lock<std::mutex> locker(mutex);
        stop = true;
    }
    condition.notify_all();
}

/**
 * 启动线程池
 */
void ThreadPool::run() {
    std::unique_lock<std::mutex> locker(mutex);
    while (!stop) {
        if (tasks.empty()) {
            condition.wait(locker);
        } else {
            HttpConnection *task = tasks.front();
            tasks.pop();
            locker.unlock();
            task->handleRequest();
            locker.lock();
        }
    }
}

/**
 * 创建线程的类静态方法
 */
void *ThreadPool::work(void *pool) {
    auto *threadPool = (ThreadPool *) pool;
    threadPool->run();
    return threadPool;
}