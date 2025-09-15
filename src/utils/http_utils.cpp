#include "http_utils.hpp"

http::response< http::string_body > utils::create_response(http::status status, bool is_error, const std::string& message)
{
  http::response<http::string_body> res(status, 11);
  res.set(http::field::content_type, "application/json");
  res.set(http::field::access_control_allow_origin, "*");

  nlohmann::json json = {
    { "error", is_error },
    { "message", message },
    { "status", static_cast< int >(status) }
  };

  res.body() = json.dump();
  res.prepare_payload();
  return res;
}

http::response< http::string_body > utils::create_json_response(http::status status, const nlohmann::json& json)
{
  http::response<http::string_body> res(status, 11);
  res.set(http::field::content_type, "application/json");
  res.set(http::field::access_control_allow_origin, "*");

  res.body() = json.dump();
  res.prepare_payload();
  return res;
}

std::vector< std::string > utils::parse_parameters(beast::string_view target)
{
  std::vector< std::string > params;

  std::string target_str(target);
  std::stringstream ss(target_str);
  while (ss.good())
  {
    std::string substr;
    std::getline(ss, substr, '/');
    params.push_back(substr);
  }

  return params;
}
