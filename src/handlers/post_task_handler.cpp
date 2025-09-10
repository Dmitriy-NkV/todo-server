#include "post_task_handler.hpp"

bool handlers::PostTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  return req.method() == http::verb::post && req.target().starts_with("/task");
}