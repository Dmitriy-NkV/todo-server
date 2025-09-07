#include "database.hpp"

database::Task::Task(int id, std::string title, std::string description, std::string status, std::chrono::system_clock::time_point created_at):
  id_(id),
  title_(title),
  description_(description),
  status_(status),
  created_at_(created_at)
{}
