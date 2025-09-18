cmake_minimum_required(VERSION 3.23.1)
project(TodoApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost 1.83.0 REQUIRED COMPONENTS system filesystem)

find_package(nlohmann_json 3.12.0 QUIET)
if(NOT nlohmann_json_FOUND)
  message(STATUS "Downloading nlohmann...")
  FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
  )
  FetchContent_MakeAvailable(nlohmann_json)
endif()

find_package(libpqxx 7.10.0 QUIET)

if(NOT libpqxx_FOUND)
  message(STATUS "Downloading libpqxx...")
  FetchContent_Declare(libpqxx
    GIT_REPOSITORY https://github.com/jtv/libpqxx.git
    GIT_TAG 7.10.0
  )
  FetchContent_MakeAvailable(libpqxx)
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/database
  ${CMAKE_CURRENT_SOURCE_DIR}/include/handlers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/server
  ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
  ${Boost_INCLUDE_DIRS}
  ${libpqxx_INCLUDE_DIRS}
)

add_subdirectory(src)

option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
