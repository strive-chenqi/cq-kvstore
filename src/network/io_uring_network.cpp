#include "io_uring_network.h"

void IoUringNetwork::init_server() {
  listen_fd_ = creat_listenfd();
  if (listen_fd_ < 0) {
    std::cerr << "Failed to create listen socket" << std::endl;
    exit(EXIT_FAILURE);
  }

  init_io_uring();
  std::cout << "Initialized server successfully" << std::endl;
}

int IoUringNetwork::set_event_recv(struct io_uring *ring, int sockfd, void *buf, size_t len, int flags) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
  if (!sqe) {
    std::cerr << "Failed to get SQE for recv event" << std::endl;
    return -1;
  }

  struct conn_info info = { .fd = sockfd, .event = EVENT_READ };
  io_uring_prep_recv(sqe, sockfd, buf, len, flags);
  memcpy(&sqe->user_data, &info, sizeof(info));

  io_uring_submit(ring); // 提交到内核

  return 0;
}

int IoUringNetwork::set_event_send(struct io_uring *ring, int sockfd, void *buf, size_t len, int flags) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
  if (!sqe) {
    std::cerr << "Failed to get SQE for send event" << std::endl;
    return -1;
  }

  struct conn_info info = { .fd = sockfd, .event = EVENT_WRITE };
  io_uring_prep_send(sqe, sockfd, buf, len, flags);
  memcpy(&sqe->user_data, &info, sizeof(info));

  io_uring_submit(ring); // 提交到内核

  return 0;
}

int IoUringNetwork::set_event_accept(struct io_uring *ring, int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
  struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
  if (!sqe) {
    std::cerr << "Failed to get SQE for accept event" << std::endl;
    return -1;
  }

  struct conn_info info = { .fd = sockfd, .event = EVENT_ACCEPT };
  io_uring_prep_accept(sqe, sockfd, addr, addrlen, flags);
  memcpy(&sqe->user_data, &info, sizeof(info));

  io_uring_submit(ring); // 提交到内核

  return 0;
}

void IoUringNetwork::run() {
  std::cout << "Running io_uring network" << std::endl;

  init_server();

  struct sockaddr_in clientaddr;
  socklen_t len = sizeof(clientaddr);
  memset(&clientaddr, 0, len);

  if (set_event_accept(&ring_, listen_fd_, (struct sockaddr *)&clientaddr, &len, 0) < 0) {
    std::cerr << "Failed to set accept event" << std::endl;
    io_uring_queue_exit(&ring_);
    close(listen_fd_);
    return;
  }

  std::cout << "Accept event set" << std::endl;

  char buffer[BUFFER_LENGTH] = {0};

  while (true) {
    struct io_uring_cqe *cqe;
    if (io_uring_wait_cqe(&ring_, &cqe) < 0) {
      perror("io_uring_wait_cqe");
      break;
    }

    struct conn_info info;
    memcpy(&info, &cqe->user_data, sizeof(info));

    if (info.event == EVENT_ACCEPT) {
      int connfd = cqe->res;
      if (connfd >= 0) {
        std::cout << "New connection accepted" << std::endl;

        if (set_event_accept(&ring_, listen_fd_, (struct sockaddr *)&clientaddr, &len, 0) < 0) {
          std::cerr << "Failed to set accept event" << std::endl;
        }

        if (set_event_recv(&ring_, connfd, buffer, BUFFER_LENGTH, 0) < 0) {
          std::cerr << "Failed to set receive event" << std::endl;
        }

      } else {
        perror("accept");
      }
    } else if (info.event == EVENT_READ) {
      int ret = cqe->res;
      if (ret <= 0) {
        close(info.fd);
        std::cout << "Connection closed or error occurred" << std::endl;
      } else {
        std::cout << "Read " << ret << " bytes: " << buffer << std::endl;

        if (set_event_send(&ring_, info.fd, buffer, ret, 0) < 0) {
          std::cerr << "Failed to set send event" << std::endl;
        }
      }
    } else if (info.event == EVENT_WRITE) {
      std::cout << "Sent data and set receive event" << std::endl;

      if (set_event_recv(&ring_, info.fd, buffer, BUFFER_LENGTH, 0) < 0) {
        std::cerr << "Failed to set receive event" << std::endl;
      }
    }

    io_uring_cqe_seen(&ring_, cqe);
  }

  io_uring_queue_exit(&ring_);
  close(listen_fd_);
}

void IoUringNetwork::send(const std::string &message) {
  std::cout << "Sending with io_uring: " << message << std::endl;
  // io_uring-specific send implementation
}

void IoUringNetwork::receive() {
  std::cout << "Receiving with io_uring" << std::endl;
  // io_uring-specific receive implementation
}

int IoUringNetwork::creat_listenfd() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return -1;
  }

  struct sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(struct sockaddr_in));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(port_);

  if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)) < 0) {
    perror("bind");
    close(sockfd);
    return -1;
  }

  if (listen(sockfd, 10) < 0) {
    perror("listen");
    close(sockfd);
    return -1;
  }

  return sockfd;
}

void IoUringNetwork::init_io_uring() {
  if (io_uring_queue_init(ENTRIES_LENGTH, &ring_, 0) < 0) {
    perror("io_uring_queue_init");
    exit(EXIT_FAILURE);
  }
}
