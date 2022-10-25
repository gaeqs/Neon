set(
        TEST_SOURCES
        main.cpp
        TestComponent.cpp
        TestComponent.h
        TestShaderController.cpp
        TestShaderController.h
        TestVertex.h
)

cmrc_add_resource_library(
        shaders
        NAMESPACE shaders
        WHENCE shader
        shader/default.vert
        shader/default.frag
)

cmrc_add_resource_library(
        resources
        NAMESPACE resources
        WHENCE resource
        resource/test.png
)

add_executable(test ${TEST_SOURCES})
target_link_libraries(test neon shaders resources)