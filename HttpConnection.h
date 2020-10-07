#ifndef WEBSERVER_HTTPCONNECTION_H
#define WEBSERVER_HTTPCONNECTION_H

#include <sys/socket.h>
#include <iostream>
#include <sys/epoll.h>
#include <cstring>
#include "Config.h"

/**
 * 管理已建立的连接
 */
class HttpConnection {

public:
    HttpConnection();

    virtual ~HttpConnection();

    void closeConnection() const;

    void handleRequest();

    void init(const int &epollFd, const int &fd);

    void setEvent(const bool &isReadEvent);

    int getFd() const;

private:
    int epollFd;
    /**
     * socket描述符
     */
    int fd;
    /**
     * HttpConnection会被放入线程池任务队列，工作线程取任务后标识判断调用Send还是Recv
     */
    bool isReadEvent;
    /**
     * 存储请求内容
     */
    char recvBuf[Config::sendRecvBufSize];
    /**
     * 下次准备接收消息的位置
     */
    unsigned long recvIndex;
    /**
     * 发送消息缓存
     */
    char sendBuf[Config::sendRecvBufSize];
    /**
     * 下次准备发送消息位置
     */
    unsigned long sendIndex;

    int webRoot;

    void handleRead();

    void handleWrite();

    bool isKeepAlive = false;
};

#endif // WEBSERVER_HTTPCONNECTION_H