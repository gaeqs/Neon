//
// Created by gaelr on 25/12/2022.
//

#include "DeferredUtils.h"
#include "engine/structure/collection/AssetCollection.h"

#include <memory>
#include <stdexcept>

#include <engine/shader/MaterialCreateInfo.h>
#include <engine/structure/Room.h>
#include <engine/render/Texture.h>
#include <engine/render/SimpleFrameBuffer.h>
#include <engine/light/DirectionalLight.h>
#include <engine/light/FlashLight.h>
#include <engine/light/PointLight.h>
#include <engine/light/LightSystem.h>

namespace neon::deferred_utils {
    struct InternalDeferredVertex {
        glm::vec2 position;

        static InputDescription getDescription() {
            InputDescription description(
                    sizeof(InternalDeferredVertex),
                    InputRate::VERTEX
            );
            description.addAttribute(2, 0);

            return description;
        }

        static InternalDeferredVertex fromAssimp(
                const glm::vec3& position,
                [[maybe_unused]] const glm::vec3& normal,
                [[maybe_unused]] const glm::vec3& tangent,
                [[maybe_unused]] const glm::vec4& color,
                [[maybe_unused]] const glm::vec2& texCoords) {
            return {position};
        }
    };


    std::shared_ptr<Model> createScreenModel(
            Room* room,
            const std::string& name,
            const std::vector<IdentifiableWrapper<Texture>>& inputTextures,
            const std::shared_ptr<FrameBuffer>& target,
            IdentifiableWrapper<ShaderProgram> shader,
            const std::function<void(
                    MaterialCreateInfo&)>& populateFunction) {

        std::vector<InternalDeferredVertex> vertices = {
                {{-1.0f, 1.0f}},
                {{1.0f,  1.0f}},
                {{1.0f,  -1.0f}},
                {{-1.0f, -1.0f}},
        };

        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        // Add an image binding for each given texture.
        std::vector<ShaderUniformBinding> materialBindings;
        materialBindings.resize(
                inputTextures.size(),
                ShaderUniformBinding{UniformBindingType::IMAGE, 0}
        );

        auto materialDescriptor = std::make_shared<ShaderUniformDescriptor>(
                room->getApplication(),
                materialBindings
        );

        MaterialCreateInfo info(target, shader);
        info.descriptions.uniform = materialDescriptor;
        info.descriptions.vertex = InternalDeferredVertex::getDescription();

        if (populateFunction != nullptr)
            populateFunction(info);

        auto material = room->getMaterials().create(info);

        for (uint32_t i = 0; i < inputTextures.size(); ++i) {
            material->getUniformBuffer().setTexture(i, inputTextures[i]);
        }

        auto mesh = std::make_unique<Mesh>(room, material);
        mesh->setMeshData(vertices, indices);
        std::vector<std::unique_ptr<Mesh>> meshes;
        meshes.push_back(std::move(mesh));


        auto& assets = room->getApplication()->getAssets();
        auto model = std::make_shared<Model>(room->getApplication(),
                                             name, meshes);
        assets.storage(model, StorageMode::WEAK);

        return model;
    }

    IdentifiableWrapper<Texture> createLightSystem(
            Room* room,
            const std::vector<IdentifiableWrapper<Texture>>& textures,
            TextureFormat outputFormat,
            IdentifiableWrapper<ShaderProgram> directionalShader,
            IdentifiableWrapper<ShaderProgram> pointShader,
            IdentifiableWrapper<ShaderProgram> flashShader) {
        std::vector<TextureFormat> outputFormatVector = {outputFormat};

        std::shared_ptr<Model> directionalModel = nullptr;
        std::shared_ptr<Model> pointModel = nullptr;
        std::shared_ptr<Model> flashModel = nullptr;

        auto frameBuffer = std::make_shared<SimpleFrameBuffer>(
                room, outputFormatVector, false);
        room->getRender().addRenderPass(
                {frameBuffer, RenderPassStrategy::defaultStrategy});

        MaterialAttachmentBlending blend;
        blend.blend = true;
        blend.colorSourceBlendFactor = BlendFactor::ONE;
        blend.colorDestinyBlendFactor = BlendFactor::ONE;

        if (directionalShader != nullptr) {
            directionalModel = createScreenModel(
                    room,
                    "Directional light model",
                    textures,
                    frameBuffer,
                    directionalShader,
                    [&blend](MaterialCreateInfo& info) {
                        info.descriptions.instance =
                                DirectionalLight::Data::getDescription();
                        info.blending.attachmentsBlending.push_back(blend);
                    }
            );
            directionalModel->defineInstanceStruct<DirectionalLight::Data>();
        }

        if (pointShader != nullptr) {
            pointModel = createScreenModel(
                    room,
                    "Point light model",
                    textures,
                    frameBuffer,
                    pointShader,
                    [&blend](MaterialCreateInfo& info) {
                        info.descriptions.instance =
                                PointLight::Data::getDescription();
                        info.blending.attachmentsBlending.push_back(blend);
                    }
            );
            pointModel->defineInstanceStruct<PointLight::Data>();
        }

        if (flashShader != nullptr) {
            flashModel = createScreenModel(
                    room,
                    "Flash light model",
                    textures,
                    frameBuffer,
                    flashShader,
                    [&blend](MaterialCreateInfo& info) {
                        info.descriptions.instance =
                                FlashLight::Data::getDescription();
                        info.blending.attachmentsBlending.push_back(blend);
                    }
            );
            flashModel->defineInstanceStruct<FlashLight::Data>();
        }

        room->newGameObject()->newComponent<LightSystem>(
                directionalModel,
                pointModel,
                flashModel
        );

        return frameBuffer->getTextures().front();
    }
}
