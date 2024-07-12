#ifndef COROUTINE_NETWORK_H
#define COROUTINE_NETWORK_H

#include "network_interface.h"

class CoroutineNetwork : public NetworkInterface {
public:
    void run() override {
        std::cout << "Running Coroutine network" << std::endl;
        // Coroutine-specific implementation
    }

    void send(const std::string& message) override {
        std::cout << "Sending with Coroutine: " << message << std::endl;
        // Coroutine-specific send implementation
    }

    void receive() override {
        std::cout << "Receiving with Coroutine" << std::endl;
        // Coroutine-specific receive implementation
    }
};



#endif // COROUTINE_NETWORK_H