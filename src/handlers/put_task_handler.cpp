#include "put_task_handler.hpp"

bool handlers::PutTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  return req.method() == http::verb::put && req.target().starts_with("/task");
}

std::unique_ptr< handlers::RequestHandler > handlers::PutTaskHandler::create() const
{
  return std::make_unique< PutTaskHandler >();
}
