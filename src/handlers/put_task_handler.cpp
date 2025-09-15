#include "put_task_handler.hpp"
#include "http_utils.hpp"

bool handlers::PutTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  auto params = utils::parse_parameters(req.target());
  return req.method() == http::verb::put && params.size() >= 2 && params[1] == "task";
}

http::response< http::string_body > handlers::PutTaskHandler::handle_request(const http::request< http::string_body >& req,
  std::shared_ptr< database::Database > db)
{
  std::vector< std::string > params = utils::parse_parameters(req.target());

  if (params.size() != 2)
  {
    return utils::create_response(http::status::bad_request, true, "Wrong parameters");
  }

  database::Task task;

  try
  {
    nlohmann::json json = nlohmann::json::parse(req.body());
    database::from_json(json, task);
  }
  catch (const nlohmann::json::parse_error&)
  {
    return utils::create_response(http::status::bad_request, true, "Wrong JSON format");
  }
  catch (const std::exception& e)
  {
    return utils::create_response(http::status::bad_request, true, e.what());
  }

  if (!task.get_id())
  {
    return utils::create_response(http::status::bad_request, true, "Wrong id");
  }

  try
  {
    db->update_task(task);
  }
  catch (const std::exception& e)
  {
    return utils::create_response(http::status::internal_server_error, true, e.what());
  }

  return utils::create_response(http::status::accepted, false, "Updated");
}

std::unique_ptr< handlers::RequestHandler > handlers::PutTaskHandler::create() const
{
  return std::make_unique< PutTaskHandler >();
}
