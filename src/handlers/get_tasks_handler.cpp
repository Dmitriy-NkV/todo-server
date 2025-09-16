#include "get_tasks_handler.hpp"
#include "http_utils.hpp"

bool handlers::GetTasksHandler::can_handle(const http::request< http::string_body >& req) const
{
  auto params = utils::parse_parameters(req.target());
  return req.method() == http::verb::get && params.size() == 2 && params[1] == "tasks";
}

http::response< http::string_body > handlers::GetTasksHandler::handle_request(const http::request< http::string_body >& req,
  std::shared_ptr< database::Database > db)
{
  std::vector< std::string > params = utils::parse_parameters(req.target());

  nlohmann::json json;
  try
  {
    json = db->get_all_tasks();
  }
  catch (const std::exception& e)
  {
    return utils::create_response(http::status::internal_server_error, true, e.what());
  }

  return utils::create_json_response(http::status::ok, json);
}

std::unique_ptr< handlers::RequestHandler > handlers::GetTasksHandler::create() const
{
  return std::make_unique< GetTasksHandler >();
}
