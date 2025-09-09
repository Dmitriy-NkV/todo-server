#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <thread>
#include "database.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace server
{
  class Listener: public std::enable_shared_from_this< Listener >
  {
  public:
    Listener(net::io_context& ioc, tcp::endpoint endpoint, database::Database& db);

    void run();

  private:
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    database::Database& db_;

    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);
  };

  class Server
  {
  public:
    Server(const std::string& host, unsigned short port, size_t threads_num, database::Database& db);
    ~Server();

    void start();
    void stop();

  private:
    std::string host_;
    unsigned short port_;
    size_t threads_num_;
    bool running_;

    net::io_context ioc_;
    std::vector< std::thread > thread_pool_;
    database::Database& db_;
  };
}

#endif