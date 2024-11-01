//
// Created by gaeqs on 25/10/2024.
//

#include "AssetLoaderCollection.h"

#include "FrameBufferLoader.h"
#include "MaterialLoader.h"
#include "RenderLoader.h"
#include "ShaderProgramLoader.h"
#include "ShaderUniformDescriptorLoader.h"
#include "TextureLoader.h"


namespace neon {
    AssetLoaderCollection::AssetLoaderCollection(bool loadDefault) {
        if (!loadDefault) return;
        registerLoader<Material, MaterialLoader>();
        registerLoader<ShaderProgram, ShaderProgramLoader>();
        registerLoader<Texture, TextureLoader>();
        registerLoader<FrameBuffer, FrameBufferLoader>();
        registerLoader<ShaderUniformDescriptor, ShaderUniformDescriptorLoader>();
        registerLoader<Render, RenderLoader>();
    }
}
