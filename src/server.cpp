#include "server.hpp"

server::Session::Session(tcp::socket&& socket, database::Database& db):
  stream_(socket),
  db_(db)
{}

server::Listener::Listener(net::io_context& ioc, database::Database& db):
  ioc_(ioc),
  acceptor_(net::make_strand(ioc)),
  db_(db)
{}

std::expected< std::shared_ptr< server::Listener >, std::string > server::Listener::create(net::io_context& ioc,
  tcp::endpoint endpoint, database::Database& db)
{
  beast::error_code ec;
  auto listener = std::make_shared< Listener >(ioc, db);

  listener->acceptor_.open(endpoint.protocol(), ec);
  if (ec)
  {
    return std::unexpected(ec.message());
  }

  listener->acceptor_.set_option(net::socket_base::reuse_address(true), ec);
  if (ec)
  {
    return std::unexpected(ec.message());
  }

  listener->acceptor_.bind(endpoint, ec);
  if (ec)
  {
    return std::unexpected(ec.message());
  }

  listener->acceptor_.listen(net::socket_base::max_listen_connections, ec);
  if (ec)
  {
    return std::unexpected(ec.message());
  }

  return listener;
}

server::Server::Server(const std::string& host, unsigned short port, size_t threads_num, database::Database& db):
  host_(host),
  port_(port),
  threads_num_(std::max(static_cast< size_t >(1), threads_num)),
  running_(false),
  ioc_(threads_num_),
  listener_(),
  thread_pool_(),
  db_(db)
{
  auto const address = net::ip::make_address(host);
  auto const endpoint = tcp::endpoint(address, port);

  auto listener = Listener::create(ioc_, endpoint, db);
  if (!listener.has_value())
  {
    throw std::runtime_error(listener.error());
  }

  listener_ = std::move(listener.value());
}

void server::Server::start()
{
  if (running_)
  {
    return;
  }
  running_ = true;

  listener_->run();

  thread_pool_.reserve(threads_num_);
  for (size_t i = 0; i != threads_num_; ++i)
  {
    thread_pool_.emplace_back([this]()
    {
      ioc_.run();
    });
  }
}

void server::Server::stop()
{
  if (!running_)
  {
    return;
  }
  running_ = false;

  ioc_.stop();
  for (size_t i = 0; i != threads_num_; ++i)
  {
    if (thread_pool_[i].joinable())
    {
      thread_pool_[i].join();
    }
  }

  thread_pool_.clear();
}