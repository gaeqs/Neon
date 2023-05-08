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
        shaders_pbr
        NAMESPACE shaders
        WHENCE shader
        shader/default.vert
        shader/default.frag
        shader/deferred.vert
        shader/deferred.frag
        shader/deferred_depth.vert
        shader/deferred_depth.frag
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
        resources_pbr
        NAMESPACE resources
        WHENCE resource
        resource/test.png
)

add_executable(testPBR ${TEST_SOURCES})
set_target_properties(testPBR PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})
target_link_libraries(testPBR neon shaders_pbr resources_pbr)
add_dependencies(testPBR tests)