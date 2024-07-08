#ifndef _POSIX_API_H_
#define _POSIX_API_H_

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <fcntl.h>
#include <sys/epoll.h>
#include <poll.h>
#include <algorithm>


class PosixApi {
public:
    enum class PollingMethod {
        SELECT = 0,
        POLL,
        EPOLL
    };

public:
    PosixApi(int port, PollingMethod method = PollingMethod::SELECT);
    ~PosixApi();
    
    void eventLoop();

private:

    void setupServer();

    void handleNewConnection();
    void handleClientData(int client_fd);

    void setNonBlocking(int fd);

    void runSelect();
    void runPoll();
    void runEpoll();
    
private:
    
    int listen_fd_;
    int port;
    PollingMethod method;

    fd_set master_set;
    int max_fd;
    std::vector<struct pollfd> poll_fds;
    int epoll_fd;
};


#endif