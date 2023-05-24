//
// Created by gaelr on 25/12/2022.
//

#ifndef NEON_DEFERREDUTILS_H
#define NEON_DEFERREDUTILS_H

#include <memory>
#include <vector>
#include <functional>

#include <engine/structure/IdentifiableWrapper.h>
#include <engine/structure/collection/AssetCollection.h>
#include <engine/model/InputDescription.h>
#include <engine/render/TextureCreateInfo.h>
#include <engine/shader/MaterialCreateInfo.h>
#include <engine/shader/ShaderUniformBinding.h>


namespace neon {
    class Application;

    class FrameBuffer;

    class ShaderProgram;

    class Texture;

    class Model;

    class Render;

    class Room;
}

namespace neon::deferred_utils {

    struct DeferredVertex {
        glm::vec2 position;

        static InputDescription getDescription() {
            InputDescription description(
                    sizeof(DeferredVertex),
                    InputRate::VERTEX
            );
            description.addAttribute(2, 0);

            return description;
        }

        static DeferredVertex fromAssimp(
                const glm::vec3& position,
                [[maybe_unused]] const glm::vec3& normal,
                [[maybe_unused]] const glm::vec3& tangent,
                [[maybe_unused]] const glm::vec4& color,
                [[maybe_unused]] const glm::vec2& texCoords) {
            return {position};
        }
    };

    std::shared_ptr<Model> createScreenModel(Application* application,
                                             const std::string& name);

    std::shared_ptr<Texture> createLightSystem(
            Room* room,
            Render* render,
            const std::vector<std::shared_ptr<Texture>>& textures,
            TextureFormat outputFormat,
            const std::shared_ptr<ShaderProgram>& directionalShader,
            const std::shared_ptr<ShaderProgram>& pointShader,
            const std::shared_ptr<ShaderProgram>& flashShader);

};


#endif //NEON_DEFERREDUTILS_H
