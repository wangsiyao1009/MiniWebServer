#include "TimeList.h"

TimeList::TimeList() : fd2NodeMap(Config::maxConnectFd), head(new TimeListNode()), tail(new TimeListNode()) {
    head->next = tail;
    tail->prev = head;
}

TimeList::~TimeList() {
    delete head;
    delete tail;
}

void TimeList::addToTail(TimeListNode *node) {
    node->next = tail;
    node->prev = tail->prev;
    tail->prev->next = node;
    tail->prev = node;
}

/**
 * 将连接添加定时器
 */
void TimeList::attachTimer(HttpConnection *connection) {
    //关闭httpConnection时默认不移除定时器，定时器过期自动移除或主线程手动移除
    removeTimer(connection->getFd());
    fd2NodeMap[connection->getFd()].expire = time(nullptr) + Config::timeoutSecond;
    fd2NodeMap[connection->getFd()].httpConnection = connection;
    addToTail(&fd2NodeMap[connection->getFd()]);
}

void TimeList::removeTimer(const int &fd) {
    auto &timeListNode = fd2NodeMap[fd];
    if (timeListNode.next == nullptr || timeListNode.prev == nullptr) return;
    timeListNode.prev->next = timeListNode.next;
    timeListNode.next->prev = timeListNode.prev;
    //标记定时器已移除
    timeListNode.next = nullptr;
    timeListNode.prev = nullptr;
}

/**
 * 到时间后删除过期连接
 */
void TimeList::tick() {
    if (head->next == tail) {
        return;
    }
    time_t current = time(nullptr);
    while (head->next != tail && head->next->expire < current) {
        auto httpConnection = head->next->httpConnection;
        removeTimer(httpConnection->getFd());
        httpConnection->closeConnection();
    }
}

/**
 * 重置定时器
 */
void TimeList::updateTimer(const int &fd) {
    auto &timeListNode = fd2NodeMap[fd];
    timeListNode.expire = time(nullptr) + Config::timeoutSecond;
    removeTimer(fd);
    addToTail(&timeListNode);
}