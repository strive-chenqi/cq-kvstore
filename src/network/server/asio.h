#ifndef _ASIO_H_
#define _ASIO_H_

#include "../glo_def.h"

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind/bind.hpp>

using namespace boost::asio;
using namespace std;

class AsioServer
{
    typedef AsioServer this_type;
    typedef ip::tcp::acceptor acceptor_type;           //接收数据
    typedef ip::tcp::endpoint endpoint_type;           //port（终端节点）
    typedef ip::tcp::socket socket_type;               //socket通信基本类
    typedef ip::address address_type;                  //ip

    typedef boost::shared_ptr<socket_type> sock_ptr;   //智能指针的typedef，它指向socket对象，用来在回调函数中传递。

private:
    io_context m_io;                                   ////asio程序必需的io_context对象
    acceptor_type m_acceptor;
    unique_ptr<io_context::work> m_work;               // io_context::work对象

    array<char, 1024> m_data; // 用于读取数据的缓冲区

public:
    AsioServer() : m_acceptor(m_io, endpoint_type(ip::tcp::v4(), 6688)), 
                   m_work(make_unique<io_context::work>(m_io)) {  // 确保 io_context 在没有事件时也不会退出
       accept();    
    }

    void run(){   
        m_io.run();  //阻塞执行事件循环 
    }  
    

private:
    void accept()
    {
        sock_ptr sock(new socket_type(m_io));
        m_acceptor.async_accept(*sock, boost::bind(&this_type::accept_handler, this, boost::asio::placeholders::error, sock)); //异步监听服务
    }

    void accept_handler(const boost::system::error_code& ec, sock_ptr sock)  //回调函数
    {
        if (ec) {     //检测错误码
            return;    
        }

        cout<<"Client:";
        cout<< sock->remote_endpoint().address() <<endl;  //远端端口的地址
        sock->async_write_some(buffer("hello asio"), boost::bind(&this_type::write_handler, this, boost::asio::placeholders::error)); //异步发送数据
        
        //sock->async_read_some(buffer(m_data), boost::bind(&this_type::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, sock));

        // 发送完毕后继续监听，否则io_service将认为没有事件处理而结束运行(加了work后不会退出，但不加accept的话处理不了事件)
        accept();
    }

    void write_handler(const boost::system::error_code&ec)
    {
        cout<<"send msg complete"<<endl;
    }

    void read_handler(const boost::system::error_code& ec, std::size_t bytes_transferred, sock_ptr sock) {
        if (ec) {
            cout << "Read error: " << ec.message() << endl;
            return;
        }

        cout << "Received: " << string(m_data.data(), bytes_transferred) << endl;

        // 继续读取数据
        sock->async_read_some(buffer(m_data), boost::bind(&this_type::read_handler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, sock));
    }
};



#endif