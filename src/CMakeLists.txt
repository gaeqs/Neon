set(SRC_SOURCES main.cpp engine/Application.cpp engine/Application.h util/Result.h gl/Shader.cpp gl/Shader.h engine/Room.cpp engine/Room.h engine/Camera.cpp engine/Camera.h engine/Frustum.cpp engine/Frustum.h engine/Renderer.cpp engine/Renderer.h engine/GraphicComponent.cpp engine/GraphicComponent.h engine/Material.cpp engine/Material.h engine/GameObject.cpp engine/GameObject.h engine/Component.cpp engine/Component.h util/ClusteredLinkedCollection.h test/TestComponent.cpp test/TestComponent.h engine/Engine.h engine/collection/ComponentCollection.cpp engine/collection/ComponentCollection.h gl/GLTexture.cpp gl/GLTexture.h engine/collection/TextureCollection.cpp engine/collection/TextureCollection.h engine/IdentifiableWrapper.h engine/Transform.cpp engine/Transform.h engine/Identifiable.h engine/Texture.cpp engine/Texture.h engine/Model.cpp engine/Model.h gl/GLModel.cpp gl/GLModel.h engine/collection/ModelCollection.h engine/collection/ModelCollection.cpp gl/GLShaderRenderer.cpp gl/GLShaderRenderer.h gl/ShaderController.cpp gl/ShaderController.h test/TestShaderController.cpp test/TestShaderController.h test/TestVertex.h)

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

add_executable(RVTracking ${SRC_SOURCES} ${SHADERS})

target_link_libraries(RVTracking shaders resources)
