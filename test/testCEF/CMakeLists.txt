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
        test_cef_resources
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
        resources/texture/skybox/skybox.json
        resources/texture/skybox/right.jpg
        resources/texture/skybox/left.jpg
        resources/texture/skybox/top.jpg
        resources/texture/skybox/bottom.jpg
        resources/texture/skybox/front.jpg
        resources/texture/skybox/back.jpg
)

add_executable(test_cef ${TEST_SOURCES})
set_target_properties(test_cef PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})
target_link_libraries(test_cef neon test_cef_resources)
add_dependencies(test_cef neon_tests)

copy_cef_resources(test_cef ${TEST_DIRECTORY})
#copy_cef_build(test_cef ${TEST_DIRECTORY})


if (MSVC)
    set_target_properties(test_cef PROPERTIES
            MSVC_RUNTIME_LIBRARY $<IF:$<CONFIG:Debug>,MultiThreadedDebug,MultiThreaded>
    )
endif ()
