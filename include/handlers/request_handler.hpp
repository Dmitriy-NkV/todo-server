#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <boost/beast/http.hpp>
#include "database.hpp"

namespace beast = boost::beast;
namespace http = beast::http;

namespace handlers
{
  class RequestHandler
  {
  public:
    virtual ~RequestHandler() = default;

    virtual bool can_handle(const http::request< http::string_body >& req) const = 0;
    virtual http::response< http::string_body > handle_request(const http::request< http::string_body >& req,
      std::shared_ptr< database::Database > db) = 0;
    virtual std::unique_ptr< RequestHandler > create() const = 0;
  };
}

#endif
