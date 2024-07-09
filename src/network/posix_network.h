#ifndef _POSIX_API_H_
#define _POSIX_API_H_

#include "network_interface.h"

class PosixNetwork : public NetworkInterface {
public:
    enum class PollingMethod {
        SELECT = 0,
        POLL,
        EPOLL
    };

public:
    PosixNetwork(int port = 9999, PollingMethod method = PollingMethod::SELECT);
    ~PosixNetwork();
    
    virtual void run() override {
        std::cout << "Running POSIX network" << std::endl;
        eventLoop();
    }

    virtual void send(const std::string& message) override {
        std::cout << "Sending with POSIX: " << message << std::endl;
        // POSIX-specific send implementation
    }

    virtual void receive() override {
        std::cout << "Receiving with POSIX" << std::endl;
        // POSIX-specific receive implementation
    }

private:
    void eventLoop();

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