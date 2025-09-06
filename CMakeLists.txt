cmake_minimum_required(VERSION 3.15)
project(PetServer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

find_package(Boost 1.83.0 REQUIRED)
find_package(PostgreSQL REQUIRED)

FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(server
  src/main.cpp
)

target_include_directories(server PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIRS}
  ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(server PRIVATE
  Boost::boost
  PostgreSQL::PostgreSQL
  nlohmann_json::nlohmann_json
  pthread
)
