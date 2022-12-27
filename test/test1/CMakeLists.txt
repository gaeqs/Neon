set(
        TEST_SOURCES
        main.cpp
        TestComponent.cpp
        TestComponent.h
        TestVertex.h
        GlobalParametersUpdaterComponent.cpp
        GlobalParametersUpdaterComponent.h
        LockMouseComponent.cpp LockMouseComponent.h ConstantRotationComponent.cpp ConstantRotationComponent.h)

cmrc_add_resource_library(
        shaders
        NAMESPACE shaders
        WHENCE shader
        shader/default.vert
        shader/default.frag
        shader/deferred.vert
        shader/deferred.frag
        shader/screen.vert
        shader/screen.frag
)

cmrc_add_resource_library(
        resources
        NAMESPACE resources
        WHENCE resource
        resource/test.png
)

add_executable(test1 ${TEST_SOURCES})
set_target_properties(test1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})
target_link_libraries(test1 neon shaders resources)
add_dependencies(test1 tests)