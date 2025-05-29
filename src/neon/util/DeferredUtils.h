//
// Created by gaelr on 25/12/2022.
//

#ifndef NEON_DEFERREDUTILS_H
#define NEON_DEFERREDUTILS_H

#include <memory>
#include <vector>

#include <rush/rush.h>

#include <neon/structure/collection/AssetCollection.h>
#include <neon/render/model/InputDescription.h>
#include <../render/texture/TextureCreateInfo.h>
#include <neon/render/shader/MaterialCreateInfo.h>
#include <neon/render/model/ModelCreateInfo.h>

namespace neon
{
    class Application;

    class FrameBuffer;

    class ShaderProgram;

    class Texture;

    class Model;

    class Render;

    class Room;
} // namespace neon

namespace neon::deferred_utils
{
    struct DeferredVertex
    {
        rush::Vec2f position;

        static InputDescription getDescription()
        {
            InputDescription description(sizeof(DeferredVertex), InputRate::VERTEX);
            description.addAttribute(2, 0);

            return description;
        }

        static DeferredVertex fromAssimp(const rush::Vec3f& position, [[maybe_unused]] const rush::Vec3f& normal,
                                         [[maybe_unused]] const rush::Vec3f& tangent,
                                         [[maybe_unused]] const rush::Vec4f& color,
                                         [[maybe_unused]] const rush::Vec2f& texCoords)
        {
            return {position};
        }
    };

    std::shared_ptr<Model> createScreenModel(Application* application, ModelCreateInfo info, const std::string& name);

    std::shared_ptr<Texture> createLightSystem(Room* room, Render* render,
                                               const std::vector<std::shared_ptr<Texture>>& textures,
                                               TextureFormat outputFormat,
                                               const std::shared_ptr<ShaderProgram>& directionalShader,
                                               const std::shared_ptr<ShaderProgram>& pointShader,
                                               const std::shared_ptr<ShaderProgram>& flashShader);
}; // namespace neon::deferred_utils

#endif //NEON_DEFERREDUTILS_H
