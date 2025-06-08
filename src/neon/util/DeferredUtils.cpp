//
// Created by gaelr on 25/12/2022.
//

#include "DeferredUtils.h"

#include <memory>
#include <utility>

#include <neon/render/shader/MaterialCreateInfo.h>
#include <neon/structure/Room.h>
#include <neon/render/buffer/SimpleFrameBuffer.h>
#include <neon/render/Render.h>
#include <neon/render/light/DirectionalLight.h>
#include <neon/render/light/FlashLight.h>
#include <neon/render/light/PointLight.h>
#include <neon/render/light/LightSystem.h>
#include <neon/render/model/Mesh.h>

namespace neon::deferred_utils
{
    std::shared_ptr<Model> createScreenModel(Application* application, ModelCreateInfo info, const std::string& name)
    {
        static std::vector<DeferredVertex> vertices = {
            {{-1.0f, 1.0f}},
            {{1.0f, 1.0f}},
            {{1.0f, -1.0f}},
            {{-1.0f, -1.0f}},
        };
        static std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        auto mesh = std::make_shared<Mesh>(application, name, std::unordered_set<std::shared_ptr<Material>>());

        mesh->uploadVertices(vertices);
        mesh->uploadIndices(indices);

        info.drawables.push_back(std::move(mesh));
        return std::make_shared<Model>(application, name, info);
    }

    std::shared_ptr<MutableAsset<TextureView>> createLightSystem(Room* room, Render* render,
                                               const std::vector<std::shared_ptr<SampledTexture>>& textures,
                                               TextureFormat outputFormat,
                                               const std::shared_ptr<ShaderProgram>& directionalShader,
                                               const std::shared_ptr<ShaderProgram>& pointShader,
                                               const std::shared_ptr<ShaderProgram>& flashShader)
    {
        auto* app = room->getApplication();

        std::vector<FrameBufferTextureCreateInfo> outputFormatVector = {outputFormat};

        std::shared_ptr<Model> directionalModel = nullptr;
        std::shared_ptr<Model> pointModel = nullptr;
        std::shared_ptr<Model> flashModel = nullptr;

        auto frameBuffer = std::make_shared<SimpleFrameBuffer>(room->getApplication(), "deferred",
                                                               SamplesPerTexel::COUNT_1, outputFormatVector);
        render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("deferred", frameBuffer));

        std::vector<ShaderUniformBinding> bindings;
        for (const auto& _ : textures) {
            bindings.push_back(ShaderUniformBinding::image());
        }

        auto uniformDescriptor = std::make_shared<ShaderUniformDescriptor>(app, "Light", bindings);

        MaterialAttachmentBlending blend;
        blend.blend = true;
        blend.colorSourceBlendFactor = BlendFactor::ONE;
        blend.colorDestinyBlendFactor = BlendFactor::ONE;

        MaterialCreateInfo info(frameBuffer, directionalShader);
        info.blending.attachmentsBlending.push_back(blend);
        info.descriptions.vertex.push_back(DeferredVertex::getDescription());
        info.descriptions.uniform = uniformDescriptor;

        if (directionalShader != nullptr) {
            info.shader = directionalShader;
            info.descriptions.instance.clear();
            info.descriptions.instance.push_back(DirectionalLight::Data::getDescription());
            ModelCreateInfo m;
            m.defineInstanceType<DirectionalLight::Data>();
            directionalModel = createScreenModel(app, m, "Directional Light");

            auto material = std::make_shared<Material>(app, "Directional Light", info);

            for (int i = 0; i < textures.size(); ++i) {
                material->getUniformBuffer()->setTexture(i, textures[i]);
            }

            directionalModel->addMaterial(material);
        }

        if (pointShader != nullptr) {
            info.shader = pointShader;
            info.descriptions.instance.clear();
            info.descriptions.instance.push_back(PointLight::Data::getDescription());
            ModelCreateInfo m;
            m.defineInstanceType<PointLight::Data>();
            pointModel = createScreenModel(app, m, "Point Light");

            auto material = std::make_shared<Material>(app, "Point Light", info);

            for (int i = 0; i < textures.size(); ++i) {
                material->getUniformBuffer()->setTexture(i, textures[i]);
            }

            pointModel->addMaterial(material);
        }

        if (flashShader != nullptr) {
            info.shader = flashShader;
            info.descriptions.instance.clear();
            info.descriptions.instance.push_back(FlashLight::Data::getDescription());

            ModelCreateInfo m;
            m.defineInstanceType<FlashLight::Data>();
            flashModel = createScreenModel(app, m, "Flash Light");

            auto material = std::make_shared<Material>(app, "Flash Light", info);

            for (int i = 0; i < textures.size(); ++i) {
                material->getUniformBuffer()->setTexture(i, textures[i]);
            }

            flashModel->addMaterial(material);
        }

        auto go = room->newGameObject();
        go->setName("Light system");
        go->newComponent<LightSystem>(directionalModel, pointModel, flashModel);

        return frameBuffer->getOutputs().front().resolvedTexture;
    }
} // namespace neon::deferred_utils
