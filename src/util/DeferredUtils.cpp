//
// Created by gaelr on 25/12/2022.
//

#include "DeferredUtils.h"

#include <memory>
#include <stdexcept>
#include <utility>

#include <engine/structure/collection/AssetCollection.h>
#include <engine/shader/MaterialCreateInfo.h>
#include <engine/structure/Room.h>
#include <engine/render/Texture.h>
#include <engine/render/SimpleFrameBuffer.h>
#include <engine/render/Render.h>
#include <engine/light/DirectionalLight.h>
#include <engine/light/FlashLight.h>
#include <engine/light/PointLight.h>
#include <engine/light/LightSystem.h>

namespace neon::deferred_utils {
    std::shared_ptr<Model> createScreenModel(Application* application,
                                             const std::string& name) {
        static std::vector<DeferredVertex> vertices = {
            {{-1.0f, 1.0f}},
            {{1.0f, 1.0f}},
            {{1.0f, -1.0f}},
            {{-1.0f, -1.0f}},
        };
        static std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        auto mesh = std::make_shared<Mesh>(
            application,
            name,
            std::unordered_set<std::shared_ptr<Material>>()
        );

        mesh->uploadVertices(vertices);
        mesh->uploadIndices(indices);

        ModelCreateInfo info;
        info.meshes.push_back(std::move(mesh));
        return std::make_shared<Model>(application, name, info);
    }

    std::shared_ptr<Texture> createLightSystem(
        Room* room,
        Render* render,
        const std::vector<std::shared_ptr<Texture>>& textures,
        TextureFormat outputFormat,
        const std::shared_ptr<ShaderProgram>& directionalShader,
        const std::shared_ptr<ShaderProgram>& pointShader,
        const std::shared_ptr<ShaderProgram>& flashShader) {
        auto* app = room->getApplication();

        std::vector<FrameBufferTextureCreateInfo> outputFormatVector =
                {outputFormat};

        std::shared_ptr<Model> directionalModel = nullptr;
        std::shared_ptr<Model> pointModel = nullptr;
        std::shared_ptr<Model> flashModel = nullptr;

        auto frameBuffer = std::make_shared<SimpleFrameBuffer>(
            room->getApplication(), outputFormatVector, false);
        render->addRenderPass(
            std::make_shared<DefaultRenderPassStrategy>(frameBuffer));

        std::vector<ShaderUniformBinding> bindings;
        for (const auto& _: textures) {
            bindings.push_back(ShaderUniformBinding::image());
        }

        auto uniformDescriptor = std::make_shared<ShaderUniformDescriptor>(
            app, "Light", bindings);

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
            info.descriptions.instance.push_back(
                DirectionalLight::Data::getDescription());
            directionalModel = createScreenModel(app, "Directional Light");
            directionalModel->defineInstanceStruct<DirectionalLight::Data>();

            auto material = std::make_shared<Material>(
                app, "Directional Light", info);

            for (int i = 0; i < textures.size(); ++i) {
                material->getUniformBuffer()->setTexture(i, textures[i]);
            }

            directionalModel->addMaterial(material);
        }

        if (pointShader != nullptr) {
            info.shader = pointShader;
            info.descriptions.instance.clear();
            info.descriptions.instance.push_back(
                PointLight::Data::getDescription());
            pointModel = createScreenModel(app, "Point Light");
            pointModel->defineInstanceStruct<PointLight::Data>();

            auto material = std::make_shared<Material>(
                app, "Point Light", info);

            for (int i = 0; i < textures.size(); ++i) {
                material->getUniformBuffer()->setTexture(i, textures[i]);
            }

            pointModel->addMaterial(material);
        }

        if (flashShader != nullptr) {
            info.shader = flashShader;
            info.descriptions.instance.clear();
            info.descriptions.instance.push_back(
                FlashLight::Data::getDescription());
            flashModel = createScreenModel(app, "Flash Light");
            flashModel->defineInstanceStruct<FlashLight::Data>();

            auto material = std::make_shared<Material>(
                app, "Flash Light", info);

            for (int i = 0; i < textures.size(); ++i) {
                material->getUniformBuffer()->setTexture(i, textures[i]);
            }

            flashModel->addMaterial(material);
        }

        auto go = room->newGameObject();
        go->setName("Light system");
        go->newComponent<LightSystem>(directionalModel, pointModel, flashModel);

        return frameBuffer->getTextures().front();
    }
}
