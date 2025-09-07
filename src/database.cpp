#include "database.hpp"

database::Task::Task(int id, std::string title, std::string description, std::string status, std::chrono::system_clock::time_point created_at):
  id_(id),
  title_(title),
  description_(description),
  status_(status),
  created_at_(created_at)
{}

void database::to_json(nlohmann::json& j, const Task& t)
{
  j = nlohmann::json{
    { "id", (*t.id_) },
    { "title", t.title_ },
    { "description", t.description_ },
    { "status", t.status_ },
    { "created_at", t.created_at_ }
  };
}

void database::from_json(const nlohmann::json& j, Task& t)
{
  database::Task task;
  if (j.contains("id"))
  {
    task.id_ = j["id"];
  }

  task.title_ = j["title"];
  task.description_ = j["description"];
  task.status_ = j["status"];

  if (j.contains("created_at"))
  {
    task.created_at_ = j["created_at"];
  }
  else
  {
    task.created_at_ = std::chrono::system_clock::now();
  }
}
