//
// Created by gaelr on 28/05/2023.
//

#include "ComputeIrradianceSkyboxComponent.h"

#include <util/DeferredUtils.h>

CMRC_DECLARE(shaders);

namespace {
    /**
     * Creates a shader program.
     * @param room the application.
     * @param name the name of the shader.
     * @param vert the vertex shader.
     * @param geom the geometry shader.
     * @param frag the fragment shader.
     * @return the shader program.
     */
    std::shared_ptr<neon::ShaderProgram> createShader(
            neon::Application* application,
            const std::string& name,
            const std::string& vert,
            const std::string& geom,
            const std::string& frag) {
        auto defaultVert = cmrc::shaders::get_filesystem().open(vert);
        auto defaultGeom = cmrc::shaders::get_filesystem().open(geom);
        auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

        auto shader = std::make_shared<neon::ShaderProgram>(application, name);
        shader->addShader(neon::ShaderType::VERTEX, defaultVert);
        shader->addShader(neon::ShaderType::GEOMETRY, defaultGeom);
        shader->addShader(neon::ShaderType::FRAGMENT, defaultFrag);

        auto result = shader->compile();

        if (result.has_value()) {
            std::cerr << result.value() << std::endl;
            throw std::runtime_error(result.value());
        }

        return shader;
    }
}

ComputeIrradianceSkyboxComponent::ComputeIrradianceSkyboxComponent(
        const std::shared_ptr<neon::Texture>& skybox,
        const std::shared_ptr<neon::Model>& screenModel) :
        _skybox(skybox),
        _screenModel(screenModel),
        _material() {}

void ComputeIrradianceSkyboxComponent::onStart() {

    neon::FrameBufferTextureCreateInfo testInfo;
    testInfo.layers = 6;
    testInfo.imageView.viewType = neon::TextureViewType::CUBE;

    auto testFrameBuffer = std::make_shared<neon::SimpleFrameBuffer>(
            getApplication(),
            std::vector<neon::FrameBufferTextureCreateInfo>{testInfo},
            false,
            [](const auto& _) { return false; },
            [](const auto& _) { return std::make_pair(1024, 1024); });


    auto irradianceShader = createShader(getApplication(),
                                         "Irradiance",
                                         "irradiance_map_creation.vert",
                                         "irradiance_map_creation.geom",
                                         "irradiance_map_creation.frag");

    // This material should not be destroyed yet.
    // We have to wait for the component to be destroyed!
    _material = neon::Material::create(
            getApplication(), "Irradiance",
            testFrameBuffer, irradianceShader,
            neon::deferred_utils::DeferredVertex::getDescription(),
            neon::InputDescription(0, neon::InputRate::INSTANCE),
            {},
            {_skybox});

    _material->getUniformBuffer()->prepareForFrame();

    auto& gub = getApplication()->getRender()->getGlobalUniformBuffer();

    _screenModel->drawOutside(_material);
    // We have to set the texture on the next frame.
    gub.setTexture(3, testFrameBuffer->getTextures()[0]);
}