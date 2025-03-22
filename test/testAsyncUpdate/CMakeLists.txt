set(
        TEST_SOURCES
        main.cpp
        TestVertex.h
        GlobalParametersUpdaterComponent.cpp
        GlobalParametersUpdaterComponent.h
        LockMouseComponent.cpp
        LockMouseComponent.h
        ConstantRotationComponent.cpp
        ConstantRotationComponent.h
        BloomRender.cpp
        BloomRender.h
        ModelAsyncFlushComponent.h
        ModelAsyncFlushComponent.cpp)

cmrc_add_resource_library(
        shaders_async_update
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
        shader/ssao.vert
        shader/ssao.frag
        shader/ssao_blur.vert
        shader/ssao_blur.frag
        shader/downsampling.vert
        shader/downsampling.frag
        shader/upsampling.vert
        shader/upsampling.frag
        shader/irradiance_map_creation.vert
        shader/irradiance_map_creation.geom
        shader/irradiance_map_creation.frag
)

cmrc_add_resource_library(
        resources_async_update
        NAMESPACE resources
        WHENCE resource
        resource/test.png
)

add_executable(test_async_update ${TEST_SOURCES})
set_target_properties(test_async_update PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})
target_link_libraries(test_async_update neon shaders_async_update resources_async_update)
add_dependencies(test_async_update neon_tests)