//
// Created by gaeqs on 25/10/2024.
//

#include "AssetLoaderCollection.h"

#include <neon/assimp/AssimpScene.h>

#include "AssimpSceneLoader.h"
#include "FrameBufferLoader.h"
#include "MaterialLoader.h"
#include "MeshLoader.h"
#include "ModelLoader.h"
#include "RenderLoader.h"
#include "ShaderProgramLoader.h"
#include "ShaderUniformDescriptorLoader.h"
#include "TextureLoader.h"
#include "RenderPassStrategyLoader.h"
#include "ShaderUniformBufferLoader.h"

namespace neon
{
    AssetLoaderCollection::AssetLoaderCollection(bool loadDefault)
    {
        if (!loadDefault) {
            return;
        }
        registerLoader<Material, MaterialLoader>();
        registerLoader<ShaderProgram, ShaderProgramLoader>();
        registerLoader<SampledTexture, TextureLoader>();
        registerLoader<FrameBuffer, FrameBufferLoader>();
        registerLoader<ShaderUniformDescriptor, ShaderUniformDescriptorLoader>();
        registerLoader<ShaderUniformBuffer, ShaderUniformBufferLoader>();
        registerLoader<RenderPassStrategy, RenderPassStrategyLoader>();
        registerLoader<Render, RenderLoader>();
        registerLoader<Mesh, MeshLoader>();
        registerLoader<AssimpScene, AssimpSceneLoader>();
        registerLoader<Model, ModelLoader>();
    }
} // namespace neon
