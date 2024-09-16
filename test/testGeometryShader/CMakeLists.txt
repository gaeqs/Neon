set(
        TEST_SOURCES
        main.cpp
        TestVertex.h
        GlobalParametersUpdaterComponent.cpp
        GlobalParametersUpdaterComponent.h
        LockMouseComponent.cpp
        LockMouseComponent.h
        ConstantRotationComponent.cpp
        ConstantRotationComponent.h)

cmrc_add_resource_library(
        shaders_geometry_shader
        NAMESPACE shaders
        WHENCE shader
        shader/default.vert
        shader/default.frag
        shader/deferred.vert
        shader/deferred.geom
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

cmrc_add_resource_library(
        resources_geometry_shader
        NAMESPACE resources
        WHENCE resource
        resource/test.png
)

add_executable(test_geometry_shader ${TEST_SOURCES})
set_target_properties(test_geometry_shader PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})
target_link_libraries(test_geometry_shader neon shaders_geometry_shader resources_geometry_shader)
add_dependencies(test_geometry_shader neon_tests)