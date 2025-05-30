find_package(Catch2 REQUIRED)

include(Catch)

project(neon-tests)
set(CMAKE_CXX_STANDARD 20)

add_executable(neon-tests task.cpp coroutine.cpp logging.cpp loader.cpp clustered_linked_collection.cpp files.cpp)

cmrc_add_resource_library(
        resources_unit
        NAMESPACE resources
        WHENCE resource
        resource/test.json
        resource/test.txt
        resource/shader.json
        resource/default.vert
        resource/default.frag
        resource/material.json
)

target_link_libraries(neon-tests PUBLIC neon Catch2::Catch2WithMain resources_unit)
set_target_properties(neon-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})

catch_discover_tests(neon-tests)