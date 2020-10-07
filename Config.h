#ifndef WEBSERVER_CONFIG_H
#define WEBSERVER_CONFIG_H

#include <unistd.h>
#include <getopt.h>
#include <cstdint>
#include <string>

class Config {

public:
    /**
     * 最多连接数
     */
    static constexpr unsigned int maxConnectFd = 65536;
    /**
     * epoll_wait调用等待的event数量
     */
    static constexpr unsigned int maxEpollWaitEvent = 10000;
    /**
     * 处理请求的发送接收缓存大小
     */
    static constexpr unsigned int sendRecvBufSize = 10240;
    /**
     * 线程池的工作线程数量
     */
    static unsigned short threadPoolNumWorkers;
    /**
     * 超时时间，自动关闭超时且无数据的连接
     */
    static constexpr unsigned short timeoutSecond = 20;
    /**
     * web目录
     */
    static const std::string webRoot;
    /**
     * 端口号
     */
    static uint16_t webServerPort;

    Config();

    virtual ~Config();

    static void parseArgs(int argc, char **argv);

};

#endif //WEBSERVER_CONFIG_H
