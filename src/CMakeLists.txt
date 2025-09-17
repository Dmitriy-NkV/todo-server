add_executable(Server
  main.cpp
  server.cpp
  database.cpp
  handler_factory.cpp
  logger.cpp
  utils/http_utils.cpp
  handlers/delete_task_handler.cpp
  handlers/get_task_handler.cpp
  handlers/get_tasks_handler.cpp
  handlers/post_task_handler.cpp
  handlers/put_task_handler.cpp
)

target_link_libraries(Server PRIVATE
  Boost::boost
  nlohmann_json::nlohmann_json
  pthread
  pqxx
)
