//
// Created by gaelr on 20/10/2022.
//

#ifndef RVTRACKING_ENGINE_H
#define RVTRACKING_ENGINE_H

#include <engine/geometry/Camera.h>
#include <engine/geometry/Frustum.h>
#include <engine/geometry/Transform.h>

#include <engine/io/CursorEvent.h>
#include <engine/io/KeyboardEvent.h>
#include <engine/io/KeyboardEvent.h>

#include <engine/light/DirectionalLight.h>
#include <engine/light/FlashLight.h>
#include <engine/light/LightSystem.h>
#include <engine/light/PointLight.h>

#include <engine/model/DefaultInstancingData.h>
#include <engine/model/InputDescription.h>
#include <engine/model/Mesh.h>
#include <engine/model/Model.h>

#include <engine/render/CommandBuffer.h>
#include <engine/render/FrameBuffer.h>
#include <engine/render/FrameBufferTextureCreateInfo.h>
#include <engine/render/FrameInformation.h>
#include <engine/render/GraphicComponent.h>
#include <engine/render/Render.h>
#include <engine/render/RenderPassStrategy.h>
#include <engine/render/SimpleFrameBuffer.h>
#include <engine/render/SwapChainFrameBuffer.h>
#include <engine/render/Texture.h>
#include <engine/render/TextureCreateInfo.h>

#include <engine/shader/Material.h>
#include <engine/shader/MaterialCreateInfo.h>
#include <engine/shader/ShaderProgram.h>
#include <engine/shader/ShaderType.h>
#include <engine/shader/ShaderUniformBinding.h>
#include <engine/shader/ShaderUniformBuffer.h>
#include <engine/shader/ShaderUniformDescriptor.h>

#include <engine/structure/Asset.h>
#include <engine/structure/Component.h>
#include <engine/structure/ComponentImplementedEvents.h>
#include <engine/structure/ComponentRegister.h>
#include <engine/structure/GameObject.h>
#include <engine/structure/Identifiable.h>
#include <engine/structure/IdentifiableWrapper.h>
#include <engine/structure/Room.h>

#include <engine/structure/collection/AssetCollection.h>
#include <engine/structure/collection/ComponentCollection.h>
#include <engine/structure/collection/IdentifiableCollection.h>


#include <engine/Application.h>

#endif //RVTRACKING_ENGINE_H
