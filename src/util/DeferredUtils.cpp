//
// Created by gaelr on 25/12/2022.
//

#include "DeferredUtils.h"

#include <stdexcept>

#include <engine/structure/Room.h>
#include <engine/render/Texture.h>
#include <engine/render/SimpleFrameBuffer.h>
#include <engine/light/DirectionalLight.h>
#include <engine/light/FlashLight.h>
#include <engine/light/PointLight.h>
#include <engine/light/LightSystem.h>

namespace deferred_utils {
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


    IdentifiableWrapper<Model> createScreenModel(
            Room* room,
            const std::vector<IdentifiableWrapper<Texture>>& inputTextures,
            const std::shared_ptr<FrameBuffer>& target,
            InputDescription instanceDescription,
            IdentifiableWrapper<ShaderProgram> shader,
            MaterialConfiguration configuration) {

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

        auto material = room->getMaterials().create(
                target,
                shader,
                materialDescriptor,
                InternalDeferredVertex::getDescription(),
                instanceDescription,
                configuration
        );

        for (uint32_t i = 0; i < inputTextures.size(); ++i) {
            material->getUniformBuffer().setTexture(i, inputTextures[i]);
        }

        auto mesh = std::make_unique<Mesh>(room, material);
        mesh->uploadVertexData(vertices, indices);
        std::vector<std::unique_ptr<Mesh>> meshes;
        meshes.push_back(std::move(mesh));

        auto model = room->getModels().create(meshes);
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

        IdentifiableWrapper<Model> directionalModel = nullptr;
        IdentifiableWrapper<Model> pointModel = nullptr;
        IdentifiableWrapper<Model> flashModel = nullptr;

        auto frameBuffer = std::make_shared<SimpleFrameBuffer>(
                room, outputFormatVector, false);
        room->getRender().addRenderPass(
                {frameBuffer, RenderPassStrategy::defaultStrategy});

        MaterialConfiguration configuration{};
        configuration.blend = true;
        configuration.colorSourceBlendFactor = BlendFactor::ONE;
        configuration.colorDestinyBlendFactor = BlendFactor::ONE;

        if (directionalShader != nullptr) {
            directionalModel = createScreenModel(
                    room,
                    textures,
                    frameBuffer,
                    DirectionalLight::Data::getDescription(),
                    directionalShader,
                    configuration
            );
            directionalModel->defineInstanceStruct<DirectionalLight::Data>();
        }

        if (pointShader != nullptr) {
            pointModel = createScreenModel(
                    room,
                    textures,
                    frameBuffer,
                    PointLight::Data::getDescription(),
                    pointShader,
                    configuration
            );
            pointModel->defineInstanceStruct<PointLight::Data>();
        }

        if (flashShader != nullptr) {
            flashModel = createScreenModel(
                    room,
                    textures,
                    frameBuffer,
                    FlashLight::Data::getDescription(),
                    flashShader,
                    configuration
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