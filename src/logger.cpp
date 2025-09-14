#include "logger.hpp"

logger::Logger& logger::Logger::get_instance()
{
  static Logger logger;
  return logger;
}

void logger::Logger::log(LogLevel level, const std::string& message)
{
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  std::string formatted_time = std::format("{:%Y-%m-%d %H:%M:%S}", *std::localtime(&time));
  std::cout << std::format("[{}] [{}]: {}\n", formatted_time, level_to_string(level), message);
}

std::string logger::Logger::level_to_string(LogLevel level)
{
  switch (level)
  {
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARNING:
      return "WARNING";
    case LogLevel::ERROR:
      return"ERROR";
    case LogLevel::CRITICAL:
      return "CRITICAL";
    default:
      return"UNKNOWN";
  }
}
