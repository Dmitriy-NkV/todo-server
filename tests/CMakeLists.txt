find_package(GoogleTest 3.12.0 QUIET)
if(NOT GoogleTest_FOUND)
  message(STATUS "Downloading GoogleTest...")
  FetchContent_Declare(GoogleTest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.17.0
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(GoogleTest)
endif()

add_executable(Tests
  ../src/database/database.cpp
  test_main.cpp
  test_database.cpp
  test_server.cpp
)

target_link_libraries(Tests PRIVATE
  Boost::boost
  nlohmann_json::nlohmann_json
  pthread
  pqxx
  gtest
  gmock
)

include(GoogleTest)
gtest_discover_tests(Tests)
