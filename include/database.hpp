#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <nlohmann/json.hpp>
#include <postgresql/libpq-fe.h>
#include <string>
#include <chrono>

namespace nlohmann {
  template<>
  struct adl_serializer< std::chrono::system_clock::time_point >
  {
    static void to_json(json& j, const std::chrono::system_clock::time_point& tp)
    {
      auto seconds = std::chrono::duration_cast< std::chrono::seconds >(tp.time_since_epoch()).count();
      j = seconds;
    }

    static void from_json(const json& j, std::chrono::system_clock::time_point& tp)
    {
      auto seconds = j.get< long long >();
      tp = std::chrono::system_clock::time_point(std::chrono::seconds(seconds));
    }
  };
}

namespace database
{
  class Task
  {
  public:
    Task() = default;
    Task(int id, std::string title, std::string description, std::string status, std::chrono::system_clock::time_point created_at);
    ~Task() = default;

    std::optional< int > get_id() const;
    std::optional< std::string > get_title() const;
    std::optional< std::string > get_description() const;
    std::optional< std::string > get_status() const;
    std::chrono::system_clock::time_point get_created_at() const;

    friend void to_json(nlohmann::json& j, const Task& t);
    friend void from_json(const nlohmann::json&, Task& t);

  private:
    std::optional< int > id_;
    std::optional< std::string > title_;
    std::optional< std::string > description_;
    std::optional< std::string > status_;
    std::chrono::system_clock::time_point created_at_;
  };

  void to_json(nlohmann::json& j, const Task& t);
  void from_json(const nlohmann::json&, Task& t);

  class Database
  {
  public:
    Database(const std::string& connection_string);
    ~Database();

    void create_task(const Task& task);
    std::vector< Task > get_all_tasks();
    std::optional< Task > get_task_by_id(const Task& task);
    void update_task(const Task& task);
    void delete_task(const Task& task);

    void initialize_database();

  private:
    std::string connection_string_;
    PGconn* connection_;

    PGresult* execute_query(const std::string& query, const std::vector< const char* >& params);
    Task result_to_task(const PGresult* res, int row);
  };
}

#endif
