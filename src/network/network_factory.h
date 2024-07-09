#ifndef NETWORK_FACTORY_H
#define NETWORK_FACTORY_H

#include "network_interface.h"
#include "posix_network.h"
#include "asio_network.h"
#include "io_uring_network.h"
#include "coroutine_network.h"

class NetworkFactory {
public:
    static std::unique_ptr<NetworkInterface> createNetwork(NetworkType type) {
        switch (type) {
            case NetworkType::POSIX:
                return std::make_unique<PosixNetwork>();
            case NetworkType::ASIO:
                return std::make_unique<AsioNetwork>();
            case NetworkType::IO_URING:
                return std::make_unique<IoUringNetwork>();
            case NetworkType::COROUTINE:
                return std::make_unique<CoroutineNetwork>();
            default:
                throw std::invalid_argument("Unknown NetworkType");
        }
    }
};

#endif // NETWORK_FACTORY_H
