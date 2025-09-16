#include "delete_task_handler.hpp"
#include "http_utils.hpp"

bool handlers::DeleteTaskHandler::can_handle(const http::request< http::string_body >& req) const
{
  auto params = utils::parse_parameters(req.target());
  return req.method() == http::verb::delete_ && params.size() == 3 && params[1] == "task";
}

http::response< http::string_body > handlers::DeleteTaskHandler::handle_request(const http::request< http::string_body >& req,
  std::shared_ptr< database::Database > db)
{
  std::vector< std::string > params = utils::parse_parameters(req.target());

  int id = 0;
  try
  {
    id = std::stoi(params[2]);
  }
  catch (const std::invalid_argument& e)
  {
    return utils::create_response(http::status::bad_request, true, "Wrong id");
  }
  catch (const std::exception& e)
  {
    return utils::create_response(http::status::bad_request, true, e.what());
  }

  database::Task task(id);

  try
  {
    db->delete_task(id);
  }
  catch (const std::exception& e)
  {
    return utils::create_response(http::status::internal_server_error, true, e.what());
  }

  return utils::create_response(http::status::accepted, false, "Deleted");
}

std::unique_ptr< handlers::RequestHandler > handlers::DeleteTaskHandler::create() const
{
  return std::make_unique< DeleteTaskHandler >();
}
