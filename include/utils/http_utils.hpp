#ifndef HTTP_UTILS_HPP
#define HTTP_UTILS_HPP

#include <boost/beast/http.hpp>
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

namespace utils
{
  http::response< http::string_body > create_response(http::status status, bool is_error, const std::string& message);

  http::response< http::string_body > create_json_response(http::status status, const nlohmann::json& json);

  std::vector< std::string > parse_parameters(beast::string_view target);

  enum class TaskStatus
  {
    TODO,
    IN_PROGRESS,
    COMPLETED,
    UNKNOWN
  };

  bool check_task_status(const std::string& status);

  TaskStatus string_to_status(const std::string& status);
}

#endif
