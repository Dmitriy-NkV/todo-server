#include "database.hpp"

database::Task::Task(int id):
  id_(id),
  title_(),
  description_(),
  status_(),
  created_at_(std::chrono::system_clock::now())
{}

database::Task::Task(int id, std::string title, std::string description, std::string status, 
  std::chrono::system_clock::time_point created_at):
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

std::optional< std::string > database::Task::get_title() const
{
  return title_;
}

std::optional< std::string > database::Task::get_description() const
{
  return description_;
}

std::optional< std::string > database::Task::get_status() const
{
  return status_;
}

std::chrono::system_clock::time_point database::Task::get_created_at() const
{
  return created_at_;
}

void database::Task::set_title(const std::string& title)
{
  title_ = title;
}

void database::Task::set_description(const std::string& description)
{
  description_ = description;
}

void database::Task::set_status(const std::string& status)
{
  status_ = status;
}

void database::to_json(nlohmann::json& j, const Task& t)
{
  auto time_t = std::chrono::system_clock::to_time_t(t.created_at_);
  std::tm tm = *std::gmtime(&time_t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

  j = nlohmann::json{
    { "id", t.id_.value() },
    { "title", t.title_.value() },
    { "description", t.description_.value() },
    { "status", t.status_.value() },
    { "created_at", oss.str() }
  };
}

void database::from_json(const nlohmann::json& j, Task& t)
{
  if (j.contains("id") && !j["id"].is_null())
  {
    t.id_ = j["id"].get< int >();
  }
  if (j.contains("title") && !j["title"].is_null())
  {
    t.title_ = j["title"].get< std::string >();
  }
  if (j.contains("description") && !j["description"].is_null())
  {
    t.description_ = j["description"].get< std::string >();
  }
  if (j.contains("status") && !j["status"].is_null())
  {
    t.status_ = j["status"].get< std::string >();
  }
  if (j.contains("created_at") && !j["created_at"].is_null())
  {
    std::string time_str = j["created_at"];
    std::tm tm = {};
    std::istringstream iss(time_str);

    iss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    if (!iss.fail())
    {
      t.created_at_ = std::chrono::system_clock::from_time_t(timegm(&tm));
    }
    else
    {
      throw std::runtime_error("Invalid date format");
    }
  }
  else
  {
    t.created_at_ = std::chrono::system_clock::now();
  }
}

database::Database::Database(const std::string& connection_string):
  connection_string_(connection_string),
  connection_(std::make_unique< pqxx::connection >(connection_string_))
{}

void database::Database::initialize_database()
{
  std::lock_guard< std::mutex > lock(db_mutex_);
  try
  {
    pqxx::work txn(*connection_);

    bool table_exists = false;
    try
    {
      auto result = txn.exec("SELECT to_regclass('public.tasks')");
      table_exists = !result[0][0].is_null();
    }
    catch (const pqxx::undefined_table&)
    {
      table_exists = false;
    }

    if (table_exists)
    {
      return;
    }

    txn.exec(R"(
      CREATE TABLE tasks (
        id SERIAL PRIMARY KEY,
        title VARCHAR(255) NOT NULL,
        description TEXT,
        status VARCHAR(50) NOT NULL,
        created_at BIGINT NOT NULL
      )
    )");

    txn.commit();
  }
  catch (const pqxx::sql_error& e)
  {
    throw std::runtime_error(e.what());
  }
}

int database::Database::create_task(const Task& task)
{
  std::lock_guard< std::mutex > lock(db_mutex_);
  try
  {
    pqxx::work txn(*connection_);

    auto timestamp = std::chrono::duration_cast< std::chrono::seconds >(
      task.get_created_at().time_since_epoch()).count();

    auto result = txn.exec(
      "INSERT INTO tasks (title, description, status, created_at) "
      "VALUES ($1, $2, $3, $4) "
      "RETURNING id",
      pqxx::params {
        task.get_title().value_or(""),
        task.get_description().value_or(""),
        task.get_status().value_or("In progress"),
        timestamp
      }
    );

    txn.commit();
    return result[0][0].as< int >();
  }
  catch (const pqxx::sql_error& e)
  {
    throw std::runtime_error(e.what());
  }
}

std::vector< database::Task > database::Database::get_all_tasks()
{
  std::vector< Task > tasks;

  try
  {
    pqxx::read_transaction txn(*connection_);

    auto result = txn.exec(
      "SELECT id, title, description, status, created_at FROM tasks "
      "ORDER BY created_at DESC"
    );

    for (size_t i = 0; i != result.size(); ++i)
    {
      tasks.push_back(row_to_task(result[i]));
    }
  }
  catch (const pqxx::sql_error& e)
  {
    throw std::runtime_error(e.what());
  }

  return tasks;
}

std::optional< database::Task > database::Database::get_task_by_id(int id)
{
  try
  {
    pqxx::read_transaction txn(*connection_);

    auto result = txn.exec(
      "SELECT id, title, description, status, created_at FROM tasks "
      "WHERE id = $1",
      pqxx::params {
        id
      }
    );

    if (result.empty())
    {
      return std::nullopt;
    }

    return row_to_task(result[0]);
  }
  catch (const pqxx::sql_error& e)
  {
    throw std::runtime_error(e.what());
  }
}

void database::Database::update_task(const Task& task)
{
  std::lock_guard< std::mutex > lock(db_mutex_);

  int id = task.get_id().value();

  auto current_task_opt = get_task_by_id(id);

  if (!current_task_opt.has_value())
  {
    throw std::runtime_error("Task with id " + std::to_string(id) + " does not exist");
  }

  auto current_task = current_task_opt.value();

  try
  {
    pqxx::work txn(*connection_);

    bool is_updated = false;

    if (auto title = task.get_title())
    {
      is_updated = true;
      current_task.set_title(title.value());
    }

    if (auto description = task.get_description())
    {
      is_updated = true;
      current_task.set_description(description.value());
    }

    if (auto status = task.get_status())
    {
      is_updated = true;
      current_task.set_status(status.value());
    }

    if (!is_updated)
    {
      throw std::invalid_argument("Nothing to update");
    }

    txn.exec(
      "UPDATE tasks SET title = $1, description = $2, status = $3 WHERE id = $4",
      pqxx::params {
        current_task.get_title().value(),
        current_task.get_description().value(),
        current_task.get_status().value(),
        id
      }
    );
    txn.commit();
  }
  catch (const pqxx::sql_error& e)
  {
    throw std::runtime_error(e.what());
  }
}

void database::Database::delete_task(int id)
{
  std::lock_guard< std::mutex > lock(db_mutex_);

  if (!check_id_exists(id))
  {
    throw std::runtime_error("Task with id " + std::to_string(id) + " does not exist");
  }

  try
  {
    pqxx::work txn(*connection_);

    txn.exec(
      "DELETE FROM tasks WHERE id = $1",
      pqxx::params {
        id
      }
    );

    txn.commit();
  }
  catch (const pqxx::sql_error& e)
  {
    throw std::runtime_error(e.what());
  }
}

database::Task database::Database::row_to_task(const pqxx::row& row) const
{
  int id = row["id"].as< int >();
  std::string title = row["title"].as< std::string >();
  std::string description = row["description"].as< std::string >("");
  std::string status = row["status"].as< std::string >();
  long long created_at_seconds = row["created_at"].as< long long >();

  auto created_at = std::chrono::system_clock::time_point(std::chrono::seconds(created_at_seconds));

  return Task(id, title, description, status, created_at);
}

bool database::Database::check_id_exists(int id) const
{
  try
  {
    pqxx::read_transaction txn(*connection_);

    auto result = txn.exec(
      "SELECT EXISTS(SELECT 1 FROM tasks WHERE id = $1)",
      pqxx::params {
        id
      }
    );

    return result[0][0].as< bool >();
  }
  catch (const pqxx::sql_error& e)
  {
    throw std::runtime_error(e.what());
  }
}
