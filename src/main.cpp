#include "./network/server/posix_api.h"
#include "./network/server/asio.h"

int main() {
    // try {
    //     PosixApi server(8080, PosixApi::PollingMethod::SELECT);
    //     server.eventLoop();
    // } catch (const std::exception& ex) {
    //     std::cerr << "Error: " << ex.what() << std::endl;
    //     return 1;
    // }
    try
    {
        std::cout<<"Server start."<<std::endl;
        AsioServer srv;
        srv.run();
    }
    catch (std::exception &e)
    {
        cout<<e.what()<<endl;
    }

    return 0;

    return 0;
}