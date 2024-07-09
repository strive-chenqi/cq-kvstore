#include "posix_network.h"

PosixNetwork::PosixNetwork(int port, PollingMethod method) : port(port), method(method), max_fd(0), epoll_fd(-1) {
    setupServer();
}

PosixNetwork::~PosixNetwork() {
    close(listen_fd_);
    if (method == PollingMethod::EPOLL) {
        close(epoll_fd);
    }
}

void PosixNetwork::setupServer() {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    int opt = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(listen_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(listen_fd_, 3) < 0) {
        throw std::runtime_error("Failed to listen on socket");
    }

    setNonBlocking(listen_fd_);

    if (method == PollingMethod::SELECT) {
        FD_ZERO(&master_set);
        FD_SET(listen_fd_, &master_set);
        max_fd = listen_fd_;
    } else if (method == PollingMethod::POLL) {
        struct pollfd pfd = { listen_fd_, POLLIN, 0 };
        poll_fds.push_back(pfd);
    } else if (method == PollingMethod::EPOLL) {
        epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
            throw std::runtime_error("Failed to create epoll file descriptor");
        }

        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = listen_fd_;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd_, &event) < 0) {
            throw std::runtime_error("Failed to add server fd to epoll");
        }
    }
}

void PosixNetwork::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        throw std::runtime_error("Failed to get file descriptor flags");
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw std::runtime_error("Failed to set non-blocking mode");
    }
}

void PosixNetwork::eventLoop() {
    if (method == PollingMethod::SELECT) {
        runSelect();
    } else if (method == PollingMethod::POLL) {
        runPoll();
    } else if (method == PollingMethod::EPOLL) {
        runEpoll();
    }
}

void PosixNetwork::handleNewConnection() {
    sockaddr_in client_address;
    socklen_t client_addrlen = sizeof(client_address);
    int client_fd = accept(listen_fd_, (struct sockaddr*)&client_address, &client_addrlen);
    if (client_fd < 0) {
        throw std::runtime_error("Failed to accept new connection");
    }
    setNonBlocking(client_fd);

    if (method == PollingMethod::SELECT) {
        FD_SET(client_fd, &master_set);
        if (client_fd > max_fd) {
            max_fd = client_fd;
        }
    } else if (method == PollingMethod::POLL) {
        struct pollfd pfd = { client_fd, POLLIN, 0 };
        poll_fds.push_back(pfd);
    } else if (method == PollingMethod::EPOLL) {
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = client_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0) {
            throw std::runtime_error("Failed to add client fd to epoll");
        }
    }
}

void PosixNetwork::handleClientData(int client_fd) {
    char buffer[1024];
    int bytes_read = read(client_fd, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        close(client_fd);
        if (method == PollingMethod::SELECT) {
            FD_CLR(client_fd, &master_set);
        } else if (method == PollingMethod::POLL) {
            poll_fds.erase(std::remove_if(poll_fds.begin(), poll_fds.end(), [client_fd](const struct pollfd& pfd) {
                return pfd.fd == client_fd;
            }), poll_fds.end());
        } else if (method == PollingMethod::EPOLL) {
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
        }
    } else {
        std::cout << "Received: " << std::string(buffer, bytes_read) << std::endl;
    }
}

void PosixNetwork::runSelect() {
    while (true) {
        fd_set read_fds = master_set;
        int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            throw std::runtime_error("select() error");
        }

        for (int i = 0; i <= max_fd; ++i) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listen_fd_) {
                    handleNewConnection();
                } else {
                    handleClientData(i);
                }
            }
        }
    }
}

void PosixNetwork::runPoll() {
    while (true) {
        int activity = poll(poll_fds.data(), poll_fds.size(), -1);
        if (activity < 0) {
            throw std::runtime_error("poll() error");
        }

        for (auto& pfd : poll_fds) {
            if (pfd.revents & POLLIN) {
                if (pfd.fd == listen_fd_) {
                    handleNewConnection();
                } else {
                    handleClientData(pfd.fd);
                }
            }
        }
    }
}

void PosixNetwork::runEpoll() {
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (n < 0) {
            throw std::runtime_error("epoll_wait() error");
        }

        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == listen_fd_) {
                handleNewConnection();
            } else {
                handleClientData(events[i].data.fd);
            }
        }
    }
}