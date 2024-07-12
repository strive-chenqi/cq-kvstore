// #include <boost/asio/co_spawn.hpp>
// #include <boost/asio/detached.hpp>
// #include <boost/asio/io_context.hpp>
// #include <boost/asio/ip/tcp.hpp>
// #include <boost/asio/signal_set.hpp>
// #include <boost/asio/write.hpp>
// #include <cstdio>
// using boost::asio::ip::tcp;
// using boost::asio::awaitable;
// using boost::asio::co_spawn;
// using boost::asio::detached;
// using boost::asio::use_awaitable;
// namespace this_coro = boost::asio::this_coro;
// #if defined(BOOST_ASIO_ENABLE_HANDLER_TRACKING)
// # define use_awaitable \
//   boost::asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
// #endif




// awaitable<void> echo(tcp::socket socket)
// {
//     try
//     {
//         char data[1024];
//         for (;;)
//         {
//             std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), use_awaitable);
//             co_await async_write(socket, boost::asio::buffer(data, n), use_awaitable);
//         }
//     }
//     catch (std::exception& e)
//     {
//         std::printf("echo Exception: %s\n", e.what());
//     }
// }
// //我们用awaitable<void>声明了一个函数，那么这个函数就变为可等待的函数了，比如listener被添加awaitable<void>之后，就可以被协程调用和等待了。
// awaitable<void> listener() 
// {
//     auto executor = co_await this_coro::executor;
//     tcp::acceptor acceptor(executor, { tcp::v4(), 10086 });
//     for (;;)
//     {
//         tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
//         //co_spawn表示启动一个协程，参数分别为调度器，执行的函数，以及启动方式, 比如我们启动了一个协程，deatched表示将协程对象分离出来，
//         //这种启动方式可以启动多个协程，他们都是独立的，如何调度取决于调度器，在用户的感知上更像是线程调度的模式，类似于并发运行，其实底层都是串行的。
//         co_spawn(executor, echo(std::move(socket)), detached);
//     }
// }
// int main()
// {
//     try
//     {
//         boost::asio::io_context io_context(1);
//         boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
//         signals.async_wait([&](auto, auto) { io_context.stop(); });
//         //我们启动了一个协程，执行listener中的逻辑，listener内部co_await 等待 acceptor接收连接，如果没有连接到来则挂起协程。
//         //当acceptor接收到连接后，继续调用co_spawn启动一个协程，用来执行echo逻辑。
//         //echo逻辑里也是通过co_wait的方式接收和发送数据的，如果对端不发数据，执行echo的协程就会挂起，另一个协程启动，继续接收新的连接。
//         //当没有连接到来，接收新连接的协程挂起，如果所有协程都挂起，则等待新的就绪事件(对端发数据，或者新连接)到来唤醒。
//         co_spawn(io_context, listener(), detached);

//         //执行之后的io_context.run()逻辑。所以协程实际上是在一个线程中串行调度的，只是感知上像是并发而已。
//         io_context.run();
//     }
    
//     catch (std::exception& e)
//     {
//         std::printf("Exception: %s\n", e.what());
//     }
// }


