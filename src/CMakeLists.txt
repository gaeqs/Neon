set(SRC_SOURCES main.cpp engine/Application.cpp engine/Application.h util/Result.h util/Resource.h util/Resource.cpp gl/Shader.cpp gl/Shader.h engine/Room.cpp engine/Room.h engine/Camera.cpp engine/Camera.h engine/Frustum.cpp engine/Frustum.h engine/Renderer.cpp engine/Renderer.h engine/GraphicComponent.cpp engine/GraphicComponent.h engine/Material.cpp engine/Material.h)
add_resources(
        SHADERS
)

add_executable(RVTracking ${SRC_SOURCES} ${SHADERS})
