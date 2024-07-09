#ifndef _IO_URING_NETWORK_H_
#define _IO_URING_NETWORK_H_

#include "network_interface.h"

#include <liburing.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#define ENTRIES_LENGTH 1024 // 队列大小
#define BUFFER_LENGTH 1024

class IoUringNetwork : public NetworkInterface {
  enum {
    EVENT_ACCEPT = 0,
    EVENT_READ,
    EVENT_WRITE
  };

  struct conn_info {
    int fd;
    int event;
  };

public:
  IoUringNetwork(int port = 9999) : port_(port), listen_fd_(-1) {}
  ~IoUringNetwork() override = default;

  void run() override;
  void send(const std::string &message) override;
  void receive() override;

private:
  void init_server();

  int creat_listenfd();
  void init_io_uring();

  int set_event_accept(struct io_uring *ring, int sockfd, struct sockaddr *addr,
                       socklen_t *addrlen, int flags);

  int set_event_recv(struct io_uring *ring, int sockfd, void *buf, size_t len,
                     int flags);

  int set_event_send(struct io_uring *ring, int sockfd, void *buf, size_t len,
                     int flags);

private:
  struct io_uring ring_;
  unsigned short port_;
  int listen_fd_;
};

#endif // _IO_URING_NETWORK_H_
