#include "post_task_handler.hpp"
#include "http_utils.hpp"

bool handlers::PostTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  auto params = utils::parse_parameters(req.target());
  return req.method() == http::verb::post && params.size() == 2 && params[1] == "task";
}

http::response< http::string_body > handlers::PostTaskHandler::handle_request(const http::request< http::string_body >& req,
  std::shared_ptr< database::Database > db)
{
  std::vector< std::string > params = utils::parse_parameters(req.target());

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

  if (task.get_id())
  {
    return utils::create_response(http::status::bad_request, true, "Wrong id");
  }
  else if (!task.get_title() || task.get_title()->empty())
  {
    return utils::create_response(http::status::bad_request, true, "Wrong title");
  }
  else if (!task.get_status() || task.get_status()->empty())
  {
    return utils::create_response(http::status::bad_request, true, "Wrong status");
  }
  else if (utils::check_task_status(task.get_status().value()))
  {
    return utils::create_response(http::status::bad_request, true, "Status must be 'Todo', 'In progress' or 'Completed'");
  }

  int id = 0;

  try
  {
    id = db->create_task(task);
  }
  catch (const std::exception& e)
  {
    return utils::create_response(http::status::internal_server_error, true, e.what());
  }

  return utils::create_response(http::status::created, false, std::to_string(id));
}

std::unique_ptr< handlers::RequestHandler > handlers::PostTaskHandler::create() const
{
  return std::make_unique< PostTaskHandler >();
}
