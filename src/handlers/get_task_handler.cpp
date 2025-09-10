#include "get_task_handler.hpp"

bool handlers::GetTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  return req.method() == http::verb::get && req.target().starts_with("/task");
}