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
        shaders_mesh_shader
        NAMESPACE shaders
        WHENCE shader
        shader/deferred.mesh
        shader/deferred.task
        shader/deferred.frag
        shader/screen.vert
        shader/screen.frag
)

cmrc_add_resource_library(
        resources_mesh_shader
        NAMESPACE resources
        WHENCE resource
        resource/test.png
)

add_executable(test_mesh_shader ${TEST_SOURCES})
set_target_properties(test_mesh_shader PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})
target_link_libraries(test_mesh_shader neon shaders_mesh_shader resources_mesh_shader)
add_dependencies(test_mesh_shader neon_tests)