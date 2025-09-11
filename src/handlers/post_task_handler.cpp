#include "post_task_handler.hpp"
#include "http_utils.hpp"

bool handlers::PostTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  return req.method() == http::verb::post && req.target().starts_with("/task");
}

http::response< http::string_body > handlers::PostTaskHandler::handle_request(const http::request< http::string_body >& req,
  std::shared_ptr< database::Database > db)
{
  std::vector< std::string > params = utils::parse_parameters(req.target());

  if (params.size() != 1)
  {
    return utils::create_error_response(http::status::bad_request, "Wrong parameters");
  }

  database::Task task;
  nlohmann::json json(req.body());
  database::from_json(json, task);

  try
  {
    db->create_task(task);
  }
  catch (const std::exception& e)
  {
    return utils::create_error_response(http::status::internal_server_error, e.what());
  }

  return utils::create_json_response(http::status::created, "Created");
}

std::unique_ptr< handlers::RequestHandler > handlers::PostTaskHandler::create() const
{
  return std::make_unique< PostTaskHandler >();
}
