#ifndef WEBSERVER_TIMELIST_H
#define WEBSERVER_TIMELIST_H

#include <vector>
#include "TimeListNode.h"

/**
 * 维护连接过期时间，定时关闭过期连接
 */
class TimeList {

public:
    TimeList();

    ~TimeList();

    void attachTimer(HttpConnection *connection);

    void removeTimer(const int &fd);

    void tick();

    void updateTimer(const int &fd);

private:
    std::vector<TimeListNode> fd2NodeMap;
    TimeListNode *head;
    TimeListNode *tail;

    void addToTail(TimeListNode *node);

};

#endif // WEBSERVER_TIMELIST_H
