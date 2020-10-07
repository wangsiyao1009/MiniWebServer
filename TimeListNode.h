#ifndef WEBSERVER_TIMELISTNODE_H
#define WEBSERVER_TIMELISTNODE_H

#include "HttpConnection.h"

class TimeListNode {

public:
    time_t expire;
    HttpConnection *httpConnection;
    TimeListNode *next;
    TimeListNode *prev;

    TimeListNode();

    virtual ~TimeListNode();
};

#endif // WEBSERVER_TIMELISTNODE_H
