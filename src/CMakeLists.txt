set(
        SOURCES
        engine/Application.cpp
        engine/Application.h
        util/Result.h
        engine/Room.cpp
        engine/Room.h
        engine/Camera.cpp
        engine/Camera.h
        engine/Frustum.cpp
        engine/Frustum.h
        engine/Renderer.cpp
        engine/Renderer.h
        engine/GraphicComponent.cpp
        engine/GraphicComponent.h
        engine/shader/Material.cpp
        engine/shader/Material.h
        engine/GameObject.cpp
        engine/GameObject.h
        engine/Component.cpp
        engine/Component.h
        util/ClusteredLinkedCollection.h
        engine/Engine.h
        engine/collection/ComponentCollection.cpp
        engine/collection/ComponentCollection.h
        engine/collection/TextureCollection.cpp
        engine/collection/TextureCollection.h
        engine/IdentifiableWrapper.h
        engine/Transform.cpp
        engine/Transform.h
        engine/Identifiable.h
        engine/Texture.cpp
        engine/Texture.h
        engine/shader/ShaderProgram.cpp
        engine/shader/ShaderProgram.h
        engine/shader/ShaderType.h
        engine/collection/IdentifiableCollection.h
        engine/model/DefaultInstancingData.h
        util/component/CameraMovementComponent.cpp
        util/component/CameraMovementComponent.h
        engine/TextureFormat.h
        assimp/ModelLoader.h
        assimp/AssimpMaterialParameters.h
        assimp/ModelLoader.cpp
        engine/KeyboardEvent.h
        engine/CursorEvent.h
        engine/model/Mesh.cpp
        engine/model/Mesh.h
        engine/model/Model.cpp
        engine/model/Model.h
        engine/shader/ShaderUniformBuffer.cpp
        engine/shader/ShaderUniformBuffer.h
        engine/shader/ShaderUniformBinding.h
        engine/shader/ShaderUniformDescriptor.cpp
        engine/shader/ShaderUniformDescriptor.h
        engine/model/InputDescription.h)


if (VULKAN_MODE)
    list(APPEND SOURCES
            vulkan/VKApplication.cpp
            vulkan/VKApplication.h
            vulkan/VKSwapChainSupportDetails.h
            vulkan/VKQueueFamilyIndices.h
            vulkan/VKTexture.cpp
            vulkan/VKTexture.h
            vulkan/buffer/Buffer.h
            vulkan/buffer/SimpleBuffer.cpp
            vulkan/buffer/SimpleBuffer.h
            vulkan/buffer/StagingBuffer.cpp
            vulkan/buffer/StagingBuffer.h
            vulkan/util/VKUtil.cpp
            vulkan/util/VKUtil.h
            vulkan/VKMesh.cpp
            vulkan/VKMesh.h
            vulkan/shader/VKShaderUniformBuffer.cpp
            vulkan/shader/VKShaderUniformBuffer.h
            vulkan/shader/VKShaderProgram.cpp
            vulkan/shader/VKShaderProgram.h
            vulkan/VKModel.cpp
            vulkan/VKModel.h
            vulkan/VKShaderRenderer.cpp
            vulkan/VKShaderRenderer.h
            vulkan/spirv/SPIRVCompiler.h
            vulkan/spirv/SPIRVCompiler.cpp
            vulkan/VKShaderUniform.h
            vulkan/shader/VKShaderUniformDescriptor.cpp
            vulkan/shader/VKShaderUniformDescriptor.h
            vulkan/shader/VKMaterial.cpp
            vulkan/shader/VKMaterial.h)
else ()
    list(APPEND SOURCES
            gl/GLTexture.cpp
            gl/GLTexture.h
            gl/GLModel.cpp
            gl/GLModel.h
            gl/GLShaderUniformBuffer.cpp
            gl/GLShaderUniformBuffer.h
            gl/GLApplication.cpp
            gl/GLApplication.h
            gl/GLShaderProgram.cpp
            gl/GLShaderProgram.h
            gl/GLMesh.cpp
            gl/GLMesh.h
            gl/GLShaderRenderer.cpp
            gl/GLShaderRenderer.h)
endif ()

cmrc_add_resource_library(dummy)

add_library(neon STATIC ${SOURCES})
target_link_libraries(neon PUBLIC ${LIBRARIES} ${CMAKE_DL_LIBS} dummy)

if (VULKAN_MODE)
    target_compile_definitions(neon PUBLIC USE_VULKAN)
else ()
    target_compile_definitions(neon PUBLIC USE_OPENGL)
endif ()