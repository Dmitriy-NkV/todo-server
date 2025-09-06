cmake_minimum_required(VERSION 3.15)
project(PetServer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost 1.70 REQUIRED COMPONENTS asio beast json)
find_package(PostgreSQL REQUIRED)

add_executable(server 
  src/main.cpp
)

target_include_directories(server PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${Boost_INCLUDE_DIRS}
  ${PostgreSQL_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  Boost::Boost
  Boost::asio
  Boost::beast
  Boost::json
  ${PostgreSQL_LIBRARIES}
  pthread
)
