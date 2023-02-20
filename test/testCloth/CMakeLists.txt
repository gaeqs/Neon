set(
        TEST_SOURCES
        main.cpp
        TestVertex.h
        GlobalParametersUpdaterComponent.cpp
        GlobalParametersUpdaterComponent.h
        LockMouseComponent.cpp
        LockMouseComponent.h
        Cloth.cpp
        Cloth.h
        cloth/ISimulable.h
        cloth/Node.h
        cloth/Node.cpp
        cloth/Spring.cpp
        cloth/Spring.h cloth/MassSpring.cpp cloth/MassSpring.h cloth/PhysicsManager.cpp cloth/PhysicsManager.h)

cmrc_add_resource_library(
        shaders_cloth
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

add_executable(testCloth ${TEST_SOURCES})
set_target_properties(testCloth PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_DIRECTORY})
target_link_libraries(testCloth neon shaders_cloth Eigen3::Eigen)
add_dependencies(testCloth tests)