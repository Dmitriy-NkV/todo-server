#ifndef GET_TASKS_HANDLER_HPP
#define GET_TASKS_HANDLER_HPP

#include "request_handler.hpp"

namespace handlers
{
  class GetTaskSHandler: public RequestHandler
  {
  public:
    bool can_handle(const http::request< http::string_body >& req) const override;
    http::response< http::string_body > handle_request(const http::request< http::string_body >& req, database::Database& db) override;
    std::unique_ptr< RequestHandler > create() const override;
  };
}

#endif
