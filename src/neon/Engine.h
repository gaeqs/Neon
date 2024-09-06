//
// Created by gaelr on 20/10/2022.
//

#ifndef RVTRACKING_ENGINE_H
#define RVTRACKING_ENGINE_H

#include <neon/geometry/Camera.h>
#include <neon/geometry/Frustum.h>
#include <neon/geometry/Transform.h>

#include <neon/io/CursorEvent.h>
#include <neon/io/KeyboardEvent.h>
#include <neon/io/MouseButtonEvent.h>
#include <neon/io/ScrollEvent.h>

#include <neon/render/light/DirectionalLight.h>
#include <neon/render/light/FlashLight.h>
#include <neon/render/light/LightSystem.h>
#include <neon/render/light/PointLight.h>

#include <neon/render/model/DefaultInstancingData.h>
#include <neon/render/model/InputDescription.h>
#include <neon/render/model/Mesh.h>
#include <neon/render/model/Model.h>

#include <neon/render/buffer/CommandBuffer.h>
#include <neon/render/buffer/CommandPool.h>
#include <neon/render/buffer/FrameBuffer.h>
#include <neon/render/buffer/FrameBufferTextureCreateInfo.h>
#include <neon/render/FrameInformation.h>
#include <neon/render/GraphicComponent.h>
#include <neon/render/Render.h>
#include <neon/render/RenderPassStrategy.h>
#include <neon/render/buffer/SimpleFrameBuffer.h>
#include <neon/render/buffer/SwapChainFrameBuffer.h>
#include <neon/render/texture/Texture.h>
#include <neon/render/texture/TextureCreateInfo.h>

#ifdef USE_QT

#include <engine/render/QTSwapChainFrameBuffer.h>

#endif

#include <neon/render/shader/Material.h>
#include <neon/render/shader/MaterialCreateInfo.h>
#include <neon/render/shader/ShaderProgram.h>
#include <neon/render/shader/ShaderType.h>
#include <neon/render/shader/ShaderUniformBinding.h>
#include <neon/render/shader/ShaderUniformBuffer.h>
#include <neon/render/shader/ShaderUniformDescriptor.h>

#include <neon/structure/Asset.h>
#include <neon/structure/Component.h>
#include <neon/structure/ComponentImplementedEvents.h>
#include <neon/structure/ComponentRegister.h>
#include <neon/structure/GameObject.h>
#include <neon/structure/Identifiable.h>
#include <neon/structure/IdentifiableWrapper.h>
#include <neon/structure/Room.h>

#include <neon/structure/collection/AssetCollection.h>
#include <neon/structure/collection/ComponentCollection.h>
#include <neon/structure/collection/IdentifiableCollection.h>


#include <neon/Application.h>

#endif //RVTRACKING_ENGINE_H
