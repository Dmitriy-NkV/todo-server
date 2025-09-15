cmake_minimum_required(VERSION 3.15)
project(TodoApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

find_package(Boost 1.75.0 REQUIRED)
find_package(PostgreSQL REQUIRED)

FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

add_library(GTest::GTest INTERFACE IMPORTED)

add_executable(server
  src/main.cpp
  src/server.cpp
  src/database.cpp
  src/handler_factory.cpp
  src/logger.cpp
  src/utils/http_utils.cpp
  src/handlers/delete_task_handler.cpp
  src/handlers/get_task_handler.cpp
  src/handlers/get_tasks_handler.cpp
  src/handlers/post_task_handler.cpp
  src/handlers/put_task_handler.cpp
)

target_include_directories(server PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/handlers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
  ${Boost_INCLUDE_DIRS}
  ${PostgreSQL_INCLUDE_DIRS}
  ${GTest_INCLUDE_DIRS}
)

target_link_libraries(server PRIVATE
  Boost::boost
  PostgreSQL::PostgreSQL
  nlohmann_json::nlohmann_json
  GTest::GTest
  pthread
)
