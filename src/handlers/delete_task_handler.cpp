#include "delete_task_handler.hpp"

bool handlers::DeleteTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  return req.method() == http::verb::delete_ && req.target().starts_with("/delete");
}

std::unique_ptr< handlers::RequestHandler > handlers::DeleteTaskHandler::create() const
{
  return std::make_unique< DeleteTaskHandler >();
}
