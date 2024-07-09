#ifndef _ASIO_H_
#define _ASIO_H_

#include "glo_def.h"
#include "network_interface.h"

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind/bind.hpp>

using namespace boost::asio;
using namespace std;

class AsioNetwork  : public NetworkInterface 
{
    typedef AsioNetwork  this_type;
    typedef ip::tcp::acceptor acceptor_type;           //接收数据
    typedef ip::tcp::endpoint endpoint_type;           //port（终端节点）
    typedef ip::tcp::socket socket_type;               //socket通信基本类
    typedef ip::address address_type;                  //ip

    typedef boost::shared_ptr<socket_type> sock_ptr;   //智能指针的typedef，它指向socket对象，用来在回调函数中传递。

public:
    AsioNetwork () : m_acceptor(m_io, endpoint_type(ip::tcp::v4(), 6688)), 
                   m_work(make_unique<io_context::work>(m_io)) {  // 确保 io_context 在没有事件时也不会退出
       accept();    
    }

    virtual void run() override;
    virtual void send(const std::string &message) override;
    virtual void receive() override;

  private:
    void accept();

    void accept_handler(const boost::system::error_code &ec,
                        sock_ptr sock); //回调函数

    void write_handler(const boost::system::error_code &ec);

    void read_handler(const boost::system::error_code &ec,
                      std::size_t bytes_transferred, sock_ptr sock);

  private:
    io_context m_io; ////asio程序必需的io_context对象
    acceptor_type m_acceptor;
    unique_ptr<io_context::work> m_work; // io_context::work对象

    array<char, 1024> m_data; // 用于读取数据的缓冲区
};



#endif