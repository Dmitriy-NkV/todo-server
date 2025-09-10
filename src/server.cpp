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
