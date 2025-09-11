#include "put_task_handler.hpp"
#include "http_utils.hpp"

bool handlers::PutTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  return req.method() == http::verb::put && req.target().starts_with("/task");
}

http::response< http::string_body > handlers::PutTaskHandler::handle_request(const http::request< http::string_body >& req,
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
    db->update_task(task);
  }
  catch (const std::exception& e)
  {
    return utils::create_error_response(http::status::internal_server_error, e.what());
  }

  return utils::create_json_response(http::status::accepted, "Updated");
}

std::unique_ptr< handlers::RequestHandler > handlers::PutTaskHandler::create() const
{
  return std::make_unique< PutTaskHandler >();
}
