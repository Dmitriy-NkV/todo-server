#include "delete_task_handler.hpp"
#include "http_utils.hpp"

bool handlers::DeleteTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  return req.method() == http::verb::delete_ && req.target().starts_with("/delete");
}

http::response< http::string_body > handlers::DeleteTaskHandler::handle_request(const http::request< http::string_body >& req,
  std::shared_ptr< database::Database > db)
{
  std::vector< std::string > params = utils::parse_parameters(req.target());

  int id = 0;
  try
  {
    if (params.size() == 2)
    {
      id = std::stoi(params[1]);
    }
    else
    {
      throw std::runtime_error("Wrong number of parameters");
    }
  }
  catch (const std::invalid_argument& e)
  {
    return utils::create_error_response(http::status::bad_request, "Wrong id");
  }
  catch (const std::exception& e)
  {
    return utils::create_error_response(http::status::bad_request, e.what());
  }

  database::Task task(id);

  try
  {
    db->delete_task(task);
  }
  catch (const std::exception& e)
  {
    return utils::create_error_response(http::status::internal_server_error, e.what());
  }

  return utils::create_json_response(http::status::accepted, "Deleted");
}

std::unique_ptr< handlers::RequestHandler > handlers::DeleteTaskHandler::create() const
{
  return std::make_unique< DeleteTaskHandler >();
}
