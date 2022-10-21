set(SRC_SOURCES main.cpp engine/Application.cpp engine/Application.h util/Result.h util/Resource.h util/Resource.cpp gl/Shader.cpp gl/Shader.h engine/Room.cpp engine/Room.h engine/Camera.cpp engine/Camera.h engine/Frustum.cpp engine/Frustum.h engine/Renderer.cpp engine/Renderer.h engine/GraphicComponent.cpp engine/GraphicComponent.h engine/Material.cpp engine/Material.h engine/GameObject.cpp engine/GameObject.h engine/Component.cpp engine/Component.h util/ClusteredLinkedCollection.h test/TestComponent.cpp test/TestComponent.h engine/Engine.h engine/ComponentsHolder.cpp engine/ComponentsHolder.h gl/Texture.cpp gl/Texture.h engine/TexturesHolder.cpp engine/TexturesHolder.h engine/ComponentWrapper.h)
add_resources(
        SHADERS
)

add_executable(RVTracking ${SRC_SOURCES} ${SHADERS})
