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


    std::shared_ptr<Model>
    createScreenModel(const ScreenModelCreationInfo& info) {
        static std::vector<InternalDeferredVertex> vertices = {
                {{-1.0f, 1.0f}},
                {{1.0f,  1.0f}},
                {{1.0f,  -1.0f}},
                {{-1.0f, -1.0f}},
        };
        static std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

        if (info.application == nullptr) {
            throw std::runtime_error("Application is null!");
        }

        std::shared_ptr<Material> material = nullptr;
        std::shared_ptr<Model> model = nullptr;
        std::shared_ptr<Mesh> mesh = nullptr;

        // Search in assets.
        if (info.searchInAssets) {
            auto& assets = info.application->getAssets();
            material = assets.get<Material>(info.materialName)
                    .value_or(nullptr);
            model = assets.get<Model>(info.modelName).value_or(nullptr);
            mesh = assets.get<Mesh>(info.modelName).value_or(nullptr);
        }


        if (material == nullptr) {
            // Add an image binding for each given texture.
            std::vector<ShaderUniformBinding> materialBindings;
            materialBindings.resize(
                    info.inputTextures.size(),
                    ShaderUniformBinding{UniformBindingType::IMAGE, 0}
            );

            auto materialDescriptor = std::make_shared<ShaderUniformDescriptor>(
                    info.application,
                    materialBindings
            );

            MaterialCreateInfo materialInfo(info.target, info.shader);
            materialInfo.descriptions.uniform = materialDescriptor;
            materialInfo.descriptions.vertex =
                    InternalDeferredVertex::getDescription();

            if (info.populateFunction != nullptr)
                info.populateFunction(materialInfo);

            material = std::make_shared<Material>(
                    info.application,
                    info.materialName,
                    materialInfo
            );

            if (info.saveInAssets) {
                info.application->getAssets().store(material, info.storageMode);
            }
        }


        for (uint32_t i = 0; i < info.inputTextures.size(); ++i) {
            material->getUniformBuffer().setTexture(i, info.inputTextures[i]);
        }

        auto mesh = std::make_shared<Mesh>(application,
                                           name, material);
        mesh->setMeshData(vertices, indices);
        std::vector<std::shared_ptr<Mesh>> meshes;
        meshes.push_back(std::move(mesh));


        auto& assets = application->getAssets();
        auto model = std::make_shared<Model>(application, name, meshes);
        assets.store(model, AssetStorageMode::PERMANENT);

        return model;
    }

    IdentifiableWrapper<Texture> createLightSystem(
            Application* application,
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
                    application,
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


    ScreenModelCreationInfo::ScreenModelCreationInfo(
            neon::Application* app_,
            const std::string& name_,
            std::vector<std::shared_ptr<Texture>> inputTextures_,
            std::shared_ptr<FrameBuffer> target_,
            std::shared_ptr<neon::ShaderProgram> shader_,
            bool searchInAssets_,
            bool saveInAssets_,
            AssetStorageMode storageMode_,
            std::function<void(MaterialCreateInfo&)> populateFunction_) :
            ScreenModelCreationInfo(
                    app_,
                    name_,
                    name_,
                    name_,
                    std::move(inputTextures_),
                    std::move(target_),
                    std::move(shader_),
                    searchInAssets_,
                    saveInAssets_,
                    storageMode_,
                    std::move(populateFunction_)) {
    }

    ScreenModelCreationInfo::ScreenModelCreationInfo(
            neon::Application* app_,
            std::string materialName_,
            std::string meshName_,
            std::string modelName_,
            std::vector<std::shared_ptr<Texture>> inputTextures_,
            std::shared_ptr<FrameBuffer> target_,
            std::shared_ptr<neon::ShaderProgram> shader_,
            bool searchInAssets_,
            bool saveInAssets_,
            AssetStorageMode storageMode_,
            std::function<void(MaterialCreateInfo&)> populateFunction_) :
            application(app_),
            materialName(std::move(materialName_)),
            meshName(std::move(meshName_)),
            modelName(std::move(modelName_)),
            inputTextures(std::move(inputTextures_)),
            target(std::move(target_)),
            shader(std::move(shader_)),
            searchInAssets(searchInAssets_),
            saveInAssets(saveInAssets_),
            storageMode(storageMode_),
            populateFunction(std::move(populateFunction_)) {}
}
