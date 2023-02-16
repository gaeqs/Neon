//
// Created by gaelr on 25/12/2022.
//

#ifndef NEON_DEFERREDUTILS_H
#define NEON_DEFERREDUTILS_H

#include <memory>
#include <vector>
#include <functional>

#include <engine/structure/IdentifiableWrapper.h>
#include <engine/model/InputDescription.h>
#include <engine/render/TextureCreateInfo.h>
#include <engine/shader/MaterialCreateInfo.h>

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
            IdentifiableWrapper<ShaderProgram> shader,
            const std::function<void(MaterialCreateInfo&)>& populateFunction
            = nullptr);

    IdentifiableWrapper<Texture> createLightSystem(
            Room* room,
            const std::vector<IdentifiableWrapper<Texture>>& textures,
            TextureFormat outputFormat,
            IdentifiableWrapper<ShaderProgram> directionalShader,
            IdentifiableWrapper<ShaderProgram> pointShader,
            IdentifiableWrapper<ShaderProgram> flashShader);

};


#endif //NEON_DEFERREDUTILS_H
