#include "handler_factory.hpp"

handlers::HandlerFactory::HandlerFactory():
  handlers_({})
{}

std::unique_ptr< handlers::RequestHandler > handlers::HandlerFactory::create_handler(const http::request< http::string_body >& req) const
{
  for (size_t i = 0; i != handlers_.size(); ++i)
  {
    if (handlers_[i]->can_handle(req))
    {
      return handlers_[i]->create();
    }
  }
  return nullptr;
}