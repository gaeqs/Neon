set(
        TEST_SOURCES
        main.cpp
        TestVertex.h
        GlobalParametersUpdaterComponent.cpp
        GlobalParametersUpdaterComponent.h
        LockMouseComponent.cpp
        LockMouseComponent.h
        Line.cpp
        Line.h)

cmrc_add_resource_library(
        shaders_line
        NAMESPACE shaders
        WHENCE shader
        shader/default.vert
        shader/default.frag
        shader/deferred.vert
        shader/deferred.frag
        shader/deferred_parallax.frag
        shader/screen.vert
        shader/screen.frag
        shader/directional_light.vert
        shader/directional_light.frag
        shader/point_light.vert
        shader/point_light.frag
        shader/flash_light.vert
        shader/flash_light.frag
)

find_package(Eigen3 CONFIG REQUIRED)

add_compile_definitions(EIGEN_INITIALIZE_MATRICES_BY_ZERO)

add_executable(test_line ${TEST_SOURCES})
set_target_properties(test_line PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})
target_link_libraries(test_line neon shaders_line Eigen3::Eigen)
add_dependencies(test_line neon_tests)