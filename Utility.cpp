#include "Utility.h"

Utility::Utility() = default;

Utility::~Utility() = default;

bool Utility::stop = false;

TimeList Utility::timeList;

std::mutex Utility::mutex;

int *Utility::pipeFd = nullptr;

void Utility::addFdToEpoll(const int &epollFd, const int &fd, const bool &oneShot) {
    setFdNonBlock(fd);//epoll ET触发需设置nonblock
    epoll_event event{0};
    event.data.fd = fd;
    event.events = EPOLLRDHUP | EPOLLET | EPOLLIN;
    if (oneShot) {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
}

int Utility::getListenFd(const uint16_t &port) {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    int optVal = 1;
    //地址复用
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, static_cast<const void *>(&optVal), sizeof(int));
    sockaddr_in address{0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    if (bind(fd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) == -1) {
        exit(EXIT_FAILURE);
    }
    listen(fd, 5);
    return fd;
}

void Utility::setFdNonBlock(const int &fd) {
    int opt = fcntl(fd, F_GETFL);
    opt |= O_NONBLOCK;
    fcntl(fd, F_SETFL, opt);
}

void Utility::setSignal(const int &sig, void (*signalHandle)(int)) {
    struct sigaction action{};
    memset(&action, 0, sizeof(action));
    action.sa_handler = signalHandle;
    sigfillset(&action.sa_mask);
    sigaction(sig, &action, nullptr);
}

void Utility::signalAlrmHandler(int sig) {
    int buf = SIGALRM;
    write(pipeFd[1], &buf, sizeof(buf));
    alarm(Config::timeoutSecond);
}

void Utility::signalSigintHandler(int sig) {
    int buf = SIGINT;
    write(pipeFd[1], &buf, sizeof(buf));
}