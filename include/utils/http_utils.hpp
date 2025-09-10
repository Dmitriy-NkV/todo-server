#ifndef HTTP_UTILS_HPP
#define HTTP_UTILS_HPP

#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

namespace utils
{
  http::response< http::string_body > create_error_responce(http::status status, const std::string& message);

  http::response< http::string_body > create_json_responce(http::status status, const nlohmann::json& json);
}

#endif
