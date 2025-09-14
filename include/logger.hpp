#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <format>
#include <chrono>

namespace logger
{
  enum class LogLevel
  {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
  };

  class Logger
  {
  public:
    Logger() = default;
    ~Logger() = default;

    static Logger& get_instance();
    void log(LogLevel level, const std::string& message);

  private:
    constexpr std::string level_to_string(LogLevel level);
  };
}

#endif
