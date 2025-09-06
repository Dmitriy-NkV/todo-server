#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <nlohmann/json.hpp>
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
      auto seconds = j.get< int64_t >();
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

    friend void to_json(nlohmann::json& j, const Task& t);
    friend void from_json(const nlohmann::json&, Task& t);

  private:
    int id_;
    std::string title_;
    std::string description_;
    std::string status_;
    std::chrono::system_clock::time_point created_at_;
  };

  void to_json(nlohmann::json& j, const Task& t);
  void from_json(const nlohmann::json&, Task& t);
}

#endif