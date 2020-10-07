#include <mutex>
#include "HttpConnection.h"
#include "Utility.h"

HttpConnection::HttpConnection()
        : epollFd(0), fd(0), isReadEvent(true), sendBuf{}, recvBuf{}, recvIndex{}, sendIndex{}, webRoot{} {
    std::string s = "HTTP/1.1 200 OK\r\n"
                    "Date: Sun, 4 Oct 2020 22:59:21 GMT\r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n"
                    "Content-Length: 87\r\n"
                    "\r\n"
                    "<html>\n"
                    "      <head></head>\n"
                    "      <body>\n"
                    "            Hello World!\n"
                    "      </body>\n"
                    "</html>\n";
    strcpy(sendBuf, s.c_str());
}

HttpConnection::~HttpConnection() = default;

void HttpConnection::closeConnection() const {
    close(fd);
}

void HttpConnection::handleRead() {
    ssize_t ret;
    while (true) {
        ret = recv(fd, recvBuf + recvIndex, Config::sendRecvBufSize - recvIndex, 0);
        if (ret == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
        } else if (ret > 0) {
            recvIndex += ret;
        }
    }
    //已收到全部消息头
    if (recvIndex >= 5 && recvBuf[recvIndex - 1] == '\n' && recvBuf[recvIndex - 2] == '\r' &&
        recvBuf[recvIndex - 3] == '\n' && recvBuf[recvIndex - 4] == '\r') {
        recvBuf[recvIndex] = '\0';
        std::cout << "Receive from socketFd: " << fd << std::endl;
        std::cout << recvBuf;
        recvIndex = 0;
        sendIndex = 0;
        epoll_event event{};
        event.events = EPOLLOUT | EPOLLONESHOT | EPOLLRDHUP;
        event.data.fd = fd;
        epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event);
    } else {//还需继续接收消息头
        epoll_event event{};
        event.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
        event.data.fd = fd;
        epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event);
    }
}

/**
 * 根据事件类型调用read或write方法
 */
void HttpConnection::handleRequest() {
    if (isReadEvent) {
        handleRead();
    } else {
        handleWrite();
    }
}

/**
 * 发送响应消息
 */
void HttpConnection::handleWrite() {
    size_t len = strlen(sendBuf);
    sendBuf[len] = '\0';
    std::cout << "Send to socketFd: " << fd << std::endl;
    std::cout << sendBuf;
    ssize_t ret = 0;
    while (sendIndex < len) {
        ret = send(fd, sendBuf + sendIndex, len - ret, 0);
        if (ret == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
        } else if (ret > 0) {
            sendIndex += ret;
        }
    }
    if (sendIndex >= len) {
        recvIndex = 0;
        sendIndex = 0;
        if (isKeepAlive) {
            epoll_event event{};
            event.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
            event.data.fd = fd;
            epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event);
        } else {//因为webbench压测工具默认发送Connection: close，需每次响应后关闭连接
            //加锁防止create fd和close fd有race condition
            std::unique_lock<std::mutex> locker(Utility::mutex);
            close(fd);
            locker.unlock();
        }
    } else { //未发送完全部消息，但缓冲区已满，需等缓冲区为空再发送
        epoll_event event{};
        event.events = EPOLLOUT | EPOLLONESHOT | EPOLLRDHUP;
        event.data.fd = fd;
        epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event);
    }
}

/**
 * 建立连接时将描述符存入connection对象
 */
void HttpConnection::init(const int &epollFd, const int &fd) {
    this->epollFd = epollFd;
    this->fd = fd;
}

void HttpConnection::setEvent(const bool &isReadEvent) {
    this->isReadEvent = isReadEvent;
}

int HttpConnection::getFd() const {
    return fd;
}