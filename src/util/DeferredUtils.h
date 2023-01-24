//
// Created by gaelr on 25/12/2022.
//

#ifndef NEON_DEFERREDUTILS_H
#define NEON_DEFERREDUTILS_H

#include <memory>
#include <vector>

#include <engine/structure/IdentifiableWrapper.h>
#include <engine/render/TextureFormat.h>

class Room;

class FrameBuffer;

class ShaderProgram;

class Texture;

class Model;

namespace deferred_utils {

    IdentifiableWrapper<Model> createScreenModel(
            Room* room,
            const std::vector<IdentifiableWrapper<Texture>>& inputTextures,
            const std::shared_ptr<FrameBuffer>& target,
            IdentifiableWrapper<ShaderProgram> shader);

    IdentifiableWrapper<Texture> createLightSystem(
            Room* room,
            IdentifiableWrapper<Texture> colorTexture,
            const std::vector<IdentifiableWrapper<Texture>>& extraTextures,
            TextureFormat outputFormat,
            IdentifiableWrapper<ShaderProgram> directionalShader,
            IdentifiableWrapper<ShaderProgram> pointShader,
            IdentifiableWrapper<ShaderProgram> flashShader);

};


#endif //NEON_DEFERREDUTILS_H
