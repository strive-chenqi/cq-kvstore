
#include "io_uring_network.h"
#include "network_factory.h"
#include <memory>

int main() {
    // try {
    //     PosixApi server(8080, PosixApi::PollingMethod::SELECT);
    //     server.eventLoop();
    // } catch (const std::exception& ex) {
    //     std::cerr << "Error: " << ex.what() << std::endl;
    //     return 1;
    // }
    // try
    // {
    //     std::cout<<"Server start."<<std::endl;
    //     AsioServer srv;
    //     srv.run();
    // }
    // catch (std::exception &e)
    // {
    //     cout<<e.what()<<endl;
    // }

    // return 0;

    // return 0;


    NetworkType type = NetworkType::IO_URING; // 选择具体的实现

    auto network = NetworkFactory::createNetwork(type);

    network->run();
    // network->send("Hello, World!");
    // network->receive();


    return 0;
}
