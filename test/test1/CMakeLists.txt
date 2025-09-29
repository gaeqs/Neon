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
        test1_resources
        NAMESPACE resources
        WHENCE resources
        resources/icon.png
        resources/default.vert
        resources/default.frag
        resources/deferred.vert
        resources/deferred.frag
        resources/deferred_parallax.frag
        resources/screen.vert
        resources/screen.frag
        resources/directional_light.vert
        resources/directional_light.frag
        resources/point_light.vert
        resources/point_light.frag
        resources/flash_light.vert
        resources/flash_light.frag
        resources/parallax_shader.json
        resources/texture/skybox/skybox.json
        resources/texture/skybox/right.jpg
        resources/texture/skybox/left.jpg
        resources/texture/skybox/top.jpg
        resources/texture/skybox/bottom.jpg
        resources/texture/skybox/front.jpg
        resources/texture/skybox/back.jpg

        resources/glsl/utils.glsl
        resources/glsl/utils_impl.glsl
)

add_executable(test1 ${TEST_SOURCES})
set_target_properties(test1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})
target_link_libraries(test1 neon test1_resources)
add_dependencies(test1 neon_tests)