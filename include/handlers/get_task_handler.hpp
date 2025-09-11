#ifndef GET_TASK_HANDLER_HPP
#define GET_TASK_HANDLER_HPP

#include "request_handler.hpp"

namespace handlers
{
  class GetTaskHandler: public RequestHandler
  {
  public:
    bool can_handle(const http::request< http::string_body >& req) const override;
    http::response< http::string_body > handle_request(const http::request< http::string_body >& req, std::shared_ptr< database::Database > db) override;
    std::unique_ptr< RequestHandler > create() const override;
  };
}

#endif
