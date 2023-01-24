//
// Created by gaelr on 25/12/2022.
//

#include "DeferredUtils.h"

#include <stdexcept>

#include <engine/structure/Room.h>
#include <engine/render/Texture.h>
#include <engine/render/SimpleFrameBuffer.h>
#include <engine/light/DirectionalLight.h>

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
                [[maybe_unused]] const glm::vec4& color,
                [[maybe_unused]] const glm::vec2& texCoords) {
            return {position};
        }
    };


    IdentifiableWrapper<Model> createScreenModel(
            Room* room,
            const std::vector<IdentifiableWrapper<Texture>>& inputTextures,
            const std::shared_ptr<FrameBuffer>& target,
            IdentifiableWrapper<ShaderProgram> shader) {

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
                InputDescription(0, InputRate::INSTANCE)
        );

        for (uint32_t i = 0; i < inputTextures.size(); ++i) {
            material->getUniformBuffer().setTexture(i, inputTextures[i]);
        }

        auto mesh = std::make_unique<Mesh>(room, material);
        mesh->uploadVertexData(vertices, indices);
        std::vector<std::unique_ptr<Mesh>> meshes;
        meshes.push_back(std::move(mesh));

        auto model = room->getModels().create(meshes);

        // Create an empty instance!
        auto instanceResult = model->createInstance();
        if (!instanceResult.isOk()) {
            throw std::runtime_error(instanceResult.getError());
        }

        return model;
    }

    IdentifiableWrapper<Texture> createLightSystem(
            Room* room,
            IdentifiableWrapper<Texture> colorTexture,
            const std::vector<IdentifiableWrapper<Texture>>& extraTextures,
            TextureFormat outputFormat,
            IdentifiableWrapper<ShaderProgram> directionalShader,
            IdentifiableWrapper<ShaderProgram> pointShader,
            IdentifiableWrapper<ShaderProgram> flashShader) {
        std::vector<TextureFormat> outputFormatVector = {outputFormat};

        std::vector<IdentifiableWrapper<Texture>> textures;
        textures.resize(extraTextures.size() + 1);
        textures[0] = colorTexture;
        for (int i = 0; i < extraTextures.size(); ++i) {
            textures[i + 1] = extraTextures[i];
        }

        if (directionalShader != nullptr) {
            auto frameBuffer = std::make_shared<SimpleFrameBuffer>(
                    room, outputFormatVector, false);

            auto model = createScreenModel(
                    room,
                    textures,
                    frameBuffer,
                    directionalShader
            );
            model->defineInstanceStruct<DirectionalLight::Data>();
            room->getRender().addRenderPass(
                    {frameBuffer, RenderPassStrategy::defaultStrategy});
            textures[0] = frameBuffer->getTextures().front();
        }

        if (pointShader != nullptr) {
            auto frameBuffer = std::make_shared<SimpleFrameBuffer>(
                    room, outputFormatVector, false);

            auto model = createScreenModel(
                    room,
                    textures,
                    frameBuffer,
                    directionalShader
            );
            //model->defineInstanceStruct<DirectionalLight::Data>();
            room->getRender().addRenderPass(
                    {frameBuffer, RenderPassStrategy::defaultStrategy});
            textures[0] = frameBuffer->getTextures().front();
        }

        if (flashShader != nullptr) {
            auto frameBuffer = std::make_shared<SimpleFrameBuffer>(
                    room, outputFormatVector, false);

            auto model = createScreenModel(
                    room,
                    textures,
                    frameBuffer,
                    directionalShader
            );
            //model->defineInstanceStruct<DirectionalLight::Data>();
            room->getRender().addRenderPass(
                    {frameBuffer, RenderPassStrategy::defaultStrategy});
            textures[0] = frameBuffer->getTextures().front();
        }

        return textures[0];
    }
}
