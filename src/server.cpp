#include "server.hpp"

server::Session::Session(tcp::socket&& socket, std::shared_ptr< database::Database > db):
  stream_(std::move(socket)),
  db_(db)
{}

void server::Session::run()
{
  net::dispatch(stream_.get_executor(), beast::bind_front_handler(&Session::do_read, shared_from_this()));
}

void server::Session::do_read()
{
  req_ = {};
  buffer_.consume(buffer_.size());
  stream_.expires_after(std::chrono::seconds(30));

  http::async_read(stream_, buffer_, req_, beast::bind_front_handler(&Session::on_read, shared_from_this()));
}

void server::Session::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
  boost::ignore_unused(bytes_transferred);

  if (ec)
  {
    log_connection_error("reading", ec);
    if (ec == http::error::end_of_stream)
    {
      return do_close();
    }
    return;
  }

  log_connection("Request");

  std::unique_ptr< handlers::RequestHandler > handler = handlers::HandlerFactory().create_handler(req_);
  if (!handler)
  {
    log_connection_error("reading", "Method not found");

    send_response(std::move(utils::create_error_response(http::status::not_found, "Not found")));
    return;
  }

  http::response< http::string_body > res;
  try
  {
    res = handler->handle_request(req_, db_);
  }
  catch (const std::exception& e)
  {
    log_connection_error("handling", e.what());

    send_response(std::move(utils::create_error_response(http::status::internal_server_error, e.what())));
    return;
  }

  send_response(std::move(res));
}

void server::Session::send_response(http::message_generator&& msg)
{
  bool keep_alive = msg.keep_alive();
  beast::async_write(stream_, std::move(msg), beast::bind_front_handler(&Session::on_write, shared_from_this(), keep_alive));
}

void server::Session::on_write(bool keep_alive, beast::error_code ec, std::size_t bytes_transferred)
{
  boost::ignore_unused(bytes_transferred);

  if (ec)
  {
    log_connection_error("writing", ec);

    return;
  }

  log_connection("Response");

  buffer_.consume(buffer_.size());

  if (!keep_alive)
  {
    return do_close();
  }

  do_read();
}

void server::Session::do_close()
{
  beast::error_code ec;
  stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
}

void server::Session::log_connection(const std::string& context)
{
  std::string log_message = std::format("{} - Method: {} Target: {}",
    context,
    std::string(req_.method_string()),
    std::string(req_.target())
  );

  LOG(logger::LogLevel::INFO, log_message);
}

void server::Session::log_connection_error(const std::string& context, const std::string& error)
{
  std::string log_message = std::format("Error in {}: {} - Method: {} Target: {}",
    context,
    error,
    std::string(req_.method_string()),
    std::string(req_.target())
  );

  LOG(logger::LogLevel::ERROR, log_message);
}

void server::Session::log_connection_error(const std::string& context, boost::beast::error_code ec)
{
  std::string log_message;
  if (ec == beast::error::timeout)
  {
    log_message = std::format("Timeout in {} - Method: {} Target: {}",
      context,
      std::string(req_.method_string()),
      std::string(req_.target())
    );

    LOG(logger::LogLevel::WARNING, log_message);
  }
  else
  {
    log_message = std::format("Error in {} - Method: {} {} Target: {}",
      context, ec.what(),
      std::string(req_.method_string()),
      std::string(req_.target())
    );
    LOG(logger::LogLevel::ERROR, log_message);
  }
}

server::Listener::Listener(net::io_context& ioc, std::shared_ptr< database::Database > db):
  ioc_(ioc),
  acceptor_(net::make_strand(ioc)),
  db_(db)
{}

void server::Listener::run()
{
  do_accept();
}

void server::Listener::do_accept()
{
  acceptor_.async_accept(net::make_strand(ioc_), beast::bind_front_handler(&Listener::on_accept, shared_from_this()));
}

void server::Listener::on_accept(beast::error_code ec, tcp::socket socket)
{
  if (ec)
  {
    std::string error = "Error in accepting: " + ec.what();
    LOG(logger::LogLevel::ERROR, error);
    return;
  }
  else
  {
    std::make_shared< Session >(std::move(socket), db_)->run();
  }

  do_accept();
}

std::expected< std::shared_ptr< server::Listener >, std::string > server::Listener::create(net::io_context& ioc,
  tcp::endpoint endpoint, std::shared_ptr< database::Database > db)
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

server::Server::Server(const std::string& host, unsigned short port, size_t threads_num, std::shared_ptr< database::Database > db):
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

server::Server::~Server()
{
  stop();
}

void server::Server::start()
{
  if (running_)
  {
    LOG(logger::LogLevel::INFO, "Server already running");
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

  LOG(logger::LogLevel::INFO, "Server started");
}

void server::Server::stop()
{
  if (!running_)
  {
    LOG(logger::LogLevel::INFO, "Server already stoped");
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

  LOG(logger::LogLevel::INFO, "Server stoped");
}
