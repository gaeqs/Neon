set(
        TEST_SOURCES
        main.cpp
        TestVertex.h
        GlobalParametersUpdaterComponent.cpp
        GlobalParametersUpdaterComponent.h
        LockMouseComponent.cpp
        LockMouseComponent.h
        BoxView.h
        OctreeView.h
        OctreeRaycastView.h
        DebugRenderComponent.h
        ObjectRaycastView.h)

cmrc_add_resource_library(
        shaders_octree
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
        shader/debug.vert
        shader/debug.frag
        shader/model.vert
        shader/model.frag
)

find_package(Eigen3 CONFIG REQUIRED)

add_compile_definitions(EIGEN_INITIALIZE_MATRICES_BY_ZERO)

add_executable(test_octree ${TEST_SOURCES})
set_target_properties(test_octree PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})
target_link_libraries(test_octree neon shaders_octree Eigen3::Eigen)
add_dependencies(test_octree neon_tests)