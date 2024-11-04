//
// Created by gaelr on 20/10/2022.
//

#ifndef RVTRACKING_ENGINE_H
#define RVTRACKING_ENGINE_H

#include <neon/assimp/AssimpGeometry.h>
#include <neon/assimp/AssimpLoader.h>
#include <neon/assimp/AssimpMaterialParameters.h>
#include <neon/assimp/AssimpNewIOSystem.h>
#include <neon/assimp/AssimpScene.h>

#include <neon/filesystem/File.h>
#include <neon/filesystem/FileSystem.h>
#include <neon/filesystem/DirectoryFileSystem.h>
#include <neon/filesystem/CMRCFileSystem.h>
#include <neon/filesystem/ZipFileSystem.h>

#include <neon/geometry/Camera.h>
#include <neon/geometry/Frustum.h>
#include <neon/geometry/Transform.h>

#include <neon/io/CursorEvent.h>
#include <neon/io/KeyboardEvent.h>
#include <neon/io/MouseButtonEvent.h>
#include <neon/io/ScrollEvent.h>

#include <neon/loader/AssetLoader.h>
#include <neon/loader/AssetLoaderCollection.h>
#include <neon/loader/AssetLoaderHelpers.h>
#include <neon/loader/ShaderProgramLoader.h>
#include <neon/loader/MaterialLoader.h>
#include <neon/loader/RenderLoader.h>
#include <neon/loader/TextureLoader.h>
#include <neon/loader/ShaderUniformDescriptorLoader.h>
#include <neon/loader/RenderLoader.h>
#include <neon/loader/MeshLoader.h>
#include <neon/loader/AssimpSceneLoader.h>

#include <neon/logging/TextEffect.h>
#include <neon/logging/Message.h>
#include <neon/logging/Logger.h>
#include <neon/logging/LogOutput.h>
#include <neon/logging/STDLogOutput.h>
#include <neon/logging/ImGuiLogOutput.h>

#include <neon/render/buffer/CommandBuffer.h>
#include <neon/render/buffer/CommandManager.h>
#include <neon/render/buffer/CommandPool.h>
#include <neon/render/buffer/FrameBuffer.h>
#include <neon/render/buffer/FrameBufferTextureCreateInfo.h>
#include <neon/render/buffer/QTSwapChainFrameBuffer.h>
#include <neon/render/buffer/SimpleFrameBuffer.h>
#include <neon/render/buffer/SwapChainFrameBuffer.h>

#include <neon/render/light/DirectionalLight.h>
#include <neon/render/light/FlashLight.h>
#include <neon/render/light/LightSystem.h>
#include <neon/render/light/PointLight.h>

#include <neon/render/model/DefaultInstancingData.h>
#include <neon/render/model/InputDescription.h>
#include <neon/render/model/InstanceData.h>
#include <neon/render/model/BasicInstanceData.h>
#include <neon/render/model/ConcurrentInstanceData.h>
#include <neon/render/model/StorageBufferInstanceData.h>
#include <neon/render/model/Mesh.h>
#include <neon/render/model/MeshShaderDrawable.h>
#include <neon/render/model/Model.h>
#include <neon/render/model/LocalModel.h>

#include <neon/render/shader/Material.h>
#include <neon/render/shader/MaterialCreateInfo.h>
#include <neon/render/shader/ShaderProgram.h>
#include <neon/render/shader/ShaderType.h>
#include <neon/render/shader/ShaderUniform.h>
#include <neon/render/shader/ShaderUniformBinding.h>
#include <neon/render/shader/ShaderUniformBuffer.h>
#include <neon/render/shader/ShaderUniformDescriptor.h>

#include <neon/render/texture/Texture.h>
#include <neon/render/texture/TextureCreateInfo.h>

#include <neon/render/FrameInformation.h>
#include <neon/render/GraphicComponent.h>
#include <neon/render/Render.h>
#include <neon/render/RenderPassStrategy.h>

#include <neon/structure/collection/AssetCollection.h>
#include <neon/structure/collection/ComponentCollection.h>
#include <neon/structure/collection/IdentifiableCollection.h>

#include <neon/structure/Asset.h>
#include <neon/structure/Component.h>
#include <neon/structure/ComponentImplementedEvents.h>
#include <neon/structure/ComponentRegister.h>
#include <neon/structure/GameObject.h>
#include <neon/structure/Identifiable.h>
#include <neon/structure/IdentifiableWrapper.h>
#include <neon/structure/Room.h>
#include <neon/structure/Application.h>
#include <neon/structure/ApplicationCreateInfo.h>

#endif //RVTRACKING_ENGINE_H
