#include "get_tasks_handler.hpp"

bool handlers::GetTasksHandler::can_handle(const http::request< http::string_body >& req) const
{
  return req.method() == http::verb::get && req.target().starts_with("/tasks");
}