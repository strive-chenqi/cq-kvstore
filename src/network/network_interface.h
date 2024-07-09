#ifndef _NETWORK_INTERFACE_H
#define _NETWORK_INTERFACE_H

#include "glo_def.h"

enum class NetworkType {
    POSIX,
    ASIO,
    IO_URING,
    COROUTINE
};

class NetworkInterface {
public:
    virtual ~NetworkInterface() = default;
    virtual void run() = 0;
    virtual void send(const std::string& message) = 0;
    virtual void receive() = 0;
};

#endif // _NETWORK_INTERFACE_H
