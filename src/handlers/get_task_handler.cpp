#include "get_task_handler.hpp"
#include "http_utils.hpp"

bool handlers::GetTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  auto params = utils::parse_parameters(req.target());
  return req.method() == http::verb::get && params.size() >= 2 && params[1] == "task";
}

http::response< http::string_body > handlers::GetTaskHandler::handle_request(const http::request< http::string_body >& req,
  std::shared_ptr< database::Database >(db))
{
  std::vector< std::string > params = utils::parse_parameters(req.target());

  int id = 0;
  try
  {
    if (params.size() == 3)
    {
      id = std::stoi(params[2]);
    }
    else
    {
      throw std::runtime_error("Wrong number of parameters");
    }
  }
  catch (const std::invalid_argument& e)
  {
    return utils::create_response(http::status::bad_request, true, "Wrong id");
  }
  catch (const std::exception& e)
  {
    return utils::create_response(http::status::bad_request, true, e.what());
  }

  nlohmann::json json;
  try
  {
    auto task = db->get_task_by_id(id);
    if (task.has_value())
    {
      json = task;
    }
  }
  catch (const std::exception& e)
  {
    return utils::create_response(http::status::internal_server_error, true, e.what());
  }

  if (json.empty())
  {
    return utils::create_response(http::status::ok, false, "No task with current id");
  }

  return utils::create_json_response(http::status::ok, json);
}

std::unique_ptr< handlers::RequestHandler > handlers::GetTaskHandler::create() const
{
  return std::make_unique< GetTaskHandler >();
}
