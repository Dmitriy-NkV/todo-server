#include "handler_factory.hpp"
#include "delete_task_handler.hpp"
#include "get_task_handler.hpp"
#include "get_tasks_handler.hpp"
#include "post_task_handler.hpp"
#include "put_task_handler.hpp"

handlers::HandlerFactory::HandlerFactory():
  handlers_({
    std::make_unique< handlers::DeleteTaskHandler >(),
    std::make_unique< handlers::GetTaskHandler >(),
    std::make_unique< handlers::GetTasksHandler >(),
    std::make_unique< handlers::PostTaskHandler >(),
    std::make_unique< handlers::PutTaskHandler >()
  })
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
