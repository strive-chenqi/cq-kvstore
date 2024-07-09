#include "asio_network.h"


void AsioNetwork::accept_handler(const boost::system::error_code &ec,
                                 sock_ptr sock) //回调函数
{
  if (ec) { //检测错误码
    return;
  }

  cout << "Client:";
  cout << sock->remote_endpoint().address() << endl; //远端端口的地址
  sock->async_write_some(buffer("hello asio"),boost::bind(&this_type::write_handler, this,boost::asio::placeholders::error)); //异步发送数据

  sock->async_read_some(buffer(m_data), boost::bind(&this_type::read_handler,this, boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred, sock));

  // 发送完毕后继续监听，否则io_service将认为没有事件处理而结束运行(加了work后不会退出，但不加accept的话处理不了事件)
  accept();
}
void AsioNetwork::read_handler(const boost::system::error_code &ec,
                               std::size_t bytes_transferred, sock_ptr sock) {
  if (ec) {
    cout << "Read error: " << ec.message() << endl;
    return;
  }

  cout << "Received: " << string(m_data.data(), bytes_transferred) << endl;

  // 继续读取数据
  sock->async_read_some(buffer(m_data),boost::bind(&this_type::read_handler, this,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred, sock));
}

void AsioNetwork::write_handler(const boost::system::error_code &ec) {
  cout << "send msg complete" << endl;
}
void AsioNetwork::accept() {
  sock_ptr sock(new socket_type(m_io));
  m_acceptor.async_accept(*sock, boost::bind(&this_type::accept_handler, this,
                                             boost::asio::placeholders::error,
                                             sock)); //异步监听服务
}

void AsioNetwork::receive() {
  std::cout << "Receiving with Asio" << std::endl;
  // Asio-specific receive implementation
}
void AsioNetwork::send(const std::string &message) {
  std::cout << "Sending with Asio: " << message << std::endl;
  // Asio-specific send implementation
}

void AsioNetwork::run() {
  std::cout << "Running Asio Network" << std::endl;
  m_io.run(); //阻塞执行事件循环
}
