#include "database.hpp"

database::Task::Task(int id, std::string title, std::string description, std::string status, std::chrono::system_clock::time_point created_at):
  id_(id),
  title_(title),
  description_(description),
  status_(status),
  created_at_(created_at)
{}

std::optional< int > database::Task::get_id() const
{
  return id_;
}

std::string database::Task::get_title() const
{
  return title_;
}

std::string database::Task::get_description() const
{
  return description_;
}

std::string database::Task::get_status() const
{
  return status_;
}

std::chrono::system_clock::time_point database::Task::get_created_at() const
{
  return created_at_;
}

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

database::Database::Database(const std::string& connection_string):
  connection_string_(connection_string),
  connection_(PQconnectdb(connection_string_.c_str()))
{
  if (PQstatus(connection_) != CONNECTION_OK)
  {
    std::string error = PQerrorMessage(connection_);
    PQfinish(connection_);
    connection_ = nullptr;
    throw std::logic_error(error);
  }
}

database::Database::~Database()
{
  PQfinish(connection_);
}
