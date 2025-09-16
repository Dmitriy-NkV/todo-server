#ifndef HANDLER_FACTORY_HPP
#define HANDLER_FACTORY_HPP

#include <memory>
#include <vector>
#include "request_handler.hpp"

namespace handlers
{
  class HandlerFactory
  {
  public:
    HandlerFactory();
    ~HandlerFactory() = default;
    std::unique_ptr< RequestHandler > create_handler(const http::request< http::string_body >& req) const;

  private:
    std::vector< std::unique_ptr< RequestHandler > > handlers_;
  };
}

#endif
