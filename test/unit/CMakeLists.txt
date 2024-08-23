find_package(Catch2 REQUIRED)

include(Catch)

project(neon-tests)
set(CMAKE_CXX_STANDARD 20)

add_executable(neon-tests task.cpp)

target_link_libraries(neon-tests PUBLIC neon Catch2::Catch2WithMain)

catch_discover_tests(neon-tests)