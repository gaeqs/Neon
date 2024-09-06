#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>
#include <random>

#include <neon/Neon.h>

#include <vulkan/VKApplication.h>

#include <neon/util/component/CameraMovementComponent.h>
#include <neon/util/component/DebugOverlayComponent.h>
#include <neon/util/component/DockSpaceComponent.h>
#include <neon/util/component/ViewportComponent.h>
#include <neon/util/component/SceneTreeComponent.h>
#include <neon/util/component/GameObjectExplorerComponent.h>
#include <neon/util/DeferredUtils.h>
#include <neon/util/ModelUtils.h>
#include <neon/assimp/AssimpLoader.h>

#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "ConstantRotationComponent.h"
#include "BloomRender.h"

constexpr int32_t WIDTH = 1000;
constexpr int32_t HEIGHT = 900;

using namespace neon;

CMRC_DECLARE(shaders);

/**
 * Creates a shader program.
 * @param room the application.
 * @param name the name of the shader.
 * @param vert the vertex shader.
 * @param frag the fragment shader.
 * @return the shader program.
 */
std::shared_ptr<ShaderProgram> createShader(Application* application,
                                            const std::string& name,
                                            const std::string& vert,
                                            const std::string& frag) {
    auto defaultVert = cmrc::shaders::get_filesystem().open(vert);
    auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

    auto result = ShaderProgram::createShader(
        application, name, defaultVert, defaultFrag);
    if (!result.isOk()) {
        std::cerr << result.getError() << std::endl;
        throw std::runtime_error(result.getError());
    }

    return result.getResult();
}

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

std::shared_ptr<Material> createSSAOMaterial(
    Room* room,
    const std::shared_ptr<FrameBuffer>& ssaoFrameBuffer,
    std::vector<std::shared_ptr<Texture>> textures) {
    constexpr uint32_t SAMPLES = 64;
    constexpr uint32_t NOISE_WIDTH = 4;

    std::random_device randomDevice;
    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
    std::default_random_engine randomGenerator(randomDevice());

    // GENERATE SAMPLES
    // We use vec4 because GLSL's vec3 has an aligment of 16 bytes.
    std::vector<rush::Vec4f> samples;
    samples.reserve(SAMPLES);
    for (uint32_t i = 0; i < SAMPLES; ++i) {
        float scale = static_cast<float>(i) / 64.0f;
        samples.push_back(
            rush::Vec4f(
                randomFloats(randomGenerator) * 2.0f - 1.0f,
                randomFloats(randomGenerator) * 2.0f - 1.0f,
                randomFloats(randomGenerator),
                0.0f
            ).normalized() * std::lerp(0.1f, 1.0f, scale)
        );
    }

    // GENERATE NOISE
    std::vector<rush::Vec3f> noise;
    noise.reserve(NOISE_WIDTH * NOISE_WIDTH);
    for (uint32_t i = 0; i < NOISE_WIDTH * NOISE_WIDTH; ++i) {
        noise.emplace_back(
            randomFloats(randomGenerator) * 2.0f - 1.0f,
            randomFloats(randomGenerator) * 2.0f - 1.0f,
            0.0f
        );
    }

    TextureCreateInfo noiseTextureCreateInfo;
    noiseTextureCreateInfo.image.format = TextureFormat::R32FG32FB32F;
    noiseTextureCreateInfo.image.width = NOISE_WIDTH;
    noiseTextureCreateInfo.image.height = NOISE_WIDTH;
    noiseTextureCreateInfo.image.depth = 1;
    noiseTextureCreateInfo.image.mipmaps = 1;
    // RTX 2070 doesn't have support for R32FG32FB32F when using optimal tiling,
    // but it does when using linear tiling.
    noiseTextureCreateInfo.image.tiling = Tiling::LINEAR;
    noiseTextureCreateInfo.sampler.anisotropy = false;
    noiseTextureCreateInfo.sampler.minificationFilter = TextureFilter::NEAREST;
    noiseTextureCreateInfo.sampler.magnificationFilter = TextureFilter::NEAREST;
    noiseTextureCreateInfo.sampler.mipmapMode = MipmapMode::NEAREST;
    noiseTextureCreateInfo.sampler.uAddressMode = AddressMode::REPEAT;
    noiseTextureCreateInfo.sampler.vAddressMode = AddressMode::REPEAT;

    auto noiseTexture = std::make_shared<Texture>(
        room->getApplication(),
        "SSAO_noise",
        noise.data(),
        noiseTextureCreateInfo
    );
    textures.push_back(noiseTexture);

    auto ssaoShader = createShader(room->getApplication(),
                                   "SSAO",
                                   "ssao.vert",
                                   "ssao.frag");

    std::shared_ptr<Material> material = Material::create(
        room->getApplication(), "SSAO",
        ssaoFrameBuffer, ssaoShader,
        deferred_utils::DeferredVertex::getDescription(),
        InputDescription(0, InputRate::INSTANCE),
        {{samples.data(), SAMPLES * sizeof(rush::Vec4f)}},
        textures);

    return material;
}


std::shared_ptr<Material> createSSAOBlurMaterial(
    Room* room,
    const std::shared_ptr<FrameBuffer>& ssaoFrameBuffer,
    const std::vector<std::shared_ptr<Texture>>& textures) {
    auto ssaoShader = createShader(room->getApplication(),
                                   "SSAO",
                                   "ssao_blur.vert",
                                   "ssao_blur.frag");

    std::shared_ptr<Material> material = Material::create(
        room->getApplication(), "SSAO Blur",
        ssaoFrameBuffer, ssaoShader,
        deferred_utils::DeferredVertex::getDescription(),
        InputDescription(0, InputRate::INSTANCE),
        {},
        textures);

    return material;
}

std::shared_ptr<Texture> computeIrradiance(
    Application* app,
    const std::shared_ptr<Model>& screenModel,
    const std::shared_ptr<Texture>& skybox) {
    neon::FrameBufferTextureCreateInfo testInfo;
    testInfo.layers = 6;
    testInfo.imageView.viewType = neon::TextureViewType::CUBE;

    auto testFrameBuffer = std::make_shared<neon::SimpleFrameBuffer>(
        app,
        std::vector<neon::FrameBufferTextureCreateInfo>{testInfo},
        false,
        [](const auto& _) { return false; },
        [](const auto& _) { return std::make_pair(1024, 1024); });


    auto irradianceShader = createShader(app,
                                         "Irradiance",
                                         "irradiance_map_creation.vert",
                                         "irradiance_map_creation.geom",
                                         "irradiance_map_creation.frag");

    // This material should not be destroyed yet.
    // We have to wait for the component to be destroyed!
    auto material = neon::Material::create(
        app, "Irradiance",
        testFrameBuffer, irradianceShader,
        neon::deferred_utils::DeferredVertex::getDescription(),
        neon::InputDescription(0, neon::InputRate::INSTANCE),
        {},
        {skybox});

    neon::CommandBuffer cf(app, true);
    cf.begin(true);

    material->getUniformBuffer()->prepareForFrame(&cf);

    auto& gub = app->getRender()->getGlobalUniformBuffer();
    gub.prepareForFrame(&cf);

    screenModel->drawOutside(material.get(), &cf);

    cf.end();
    cf.submit();

    // We have to set the texture on the next frame.
    return testFrameBuffer->getTextures()[0];
}

std::shared_ptr<FrameBuffer> initRender(
    Room* room,
    const std::shared_ptr<Model>& screenModel) {
    auto* app = room->getApplication();

    // Bindings for the global uniforms.
    // In this application, we have a buffer of global parameters
    // and two textures: a skybox and an irradiance skybox.
    std::vector<ShaderUniformBinding> globalBindings = {
        {UniformBindingType::BUFFER, sizeof(Matrices)},
        {UniformBindingType::BUFFER, sizeof(PBRParameters)},
        {UniformBindingType::IMAGE, 0},
        {UniformBindingType::IMAGE, 0},
        {UniformBindingType::IMAGE, 0}
    };

    // The description of the global uniforms.
    auto globalDescriptor = std::make_shared<ShaderUniformDescriptor>(
        app, "default", globalBindings);

    // The render of the application.
    // We should set the render to the application before
    // we do anything else, as some components depend on
    // the application's render.
    auto render = std::make_shared<Render>(app, "default", globalDescriptor);
    app->setRender(render);

    auto directionalShader = createShader(app,
                                          "directional_light",
                                          "directional_light.vert",
                                          "directional_light.frag");
    auto pointShader = createShader(app,
                                    "point_light",
                                    "point_light.vert",
                                    "point_light.frag");
    auto flashShader = createShader(app,
                                    "flash_light",
                                    "flash_light.vert",
                                    "flash_light.frag");
    auto screenShader = createShader(app,
                                     "screen",
                                     "screen.vert",
                                     "screen.frag");
    auto downsampling = createShader(app,
                                     "downsampling",
                                     "downsampling.vert",
                                     "downsampling.frag");
    auto upsampling = createShader(app,
                                   "upsampling",
                                   "upsampling.vert",
                                   "upsampling.frag");

    // G BUFFER

    std::vector<FrameBufferTextureCreateInfo> frameBufferFormats = {
        TextureFormat::R16FG16FB16FA16F, // ALBEDO
        TextureFormat::R16FG16F, // NORMAL XY
        TextureFormat::R16FG16F // METALLIC / ROUGHNESS
    };

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(app,
        frameBufferFormats, /*depth buffer?*/
        true);

    render->addRenderPass(
        std::make_shared<DefaultRenderPassStrategy>(fpFrameBuffer));

    // Out textures. You can use these in other frame buffers.
    auto fpTextures = fpFrameBuffer->getTextures();

    // SSAO

    std::vector<FrameBufferTextureCreateInfo> ssaoTextureInfo{
        TextureFormat::R32F
    };
    ssaoTextureInfo[0].sampler.anisotropy = false;
    ssaoTextureInfo[0].sampler.magnificationFilter = TextureFilter::LINEAR;
    ssaoTextureInfo[0].sampler.minificationFilter = TextureFilter::LINEAR;

    auto ssaoFrameBuffer = std::make_shared<SimpleFrameBuffer>(
        app, ssaoTextureInfo,
        false,
        neon::SimpleFrameBuffer::defaultRecreationCondition,
        neon::SimpleFrameBuffer::defaultRecreationParameters
    );

    app->getRender()->addRenderPass(
        std::make_shared<DefaultRenderPassStrategy>(ssaoFrameBuffer));

    auto ssaoBlurFrameBuffer = std::make_shared<SimpleFrameBuffer>(
        app, ssaoTextureInfo,
        false,
        neon::SimpleFrameBuffer::defaultRecreationCondition,
        neon::SimpleFrameBuffer::defaultRecreationParameters
    );

    app->getRender()->addRenderPass(
        std::make_shared<DefaultRenderPassStrategy>(ssaoBlurFrameBuffer));


    auto ssaoMaterial = createSSAOMaterial(
        room, ssaoFrameBuffer,
        {
            fpTextures.at(0), // ALBEDO
            fpTextures.at(1), // NORMAL XY
            fpTextures.at(3) // DEPTH
        }
    );

    auto ssaoBlurMaterial = createSSAOBlurMaterial(
        room, ssaoBlurFrameBuffer, ssaoFrameBuffer->getTextures());

    // LIGHT SYSTEM

    auto albedo = deferred_utils::createLightSystem(
        room,
        render.get(),
        fpTextures,
        TextureFormat::R16FG16FB16FA16F,
        directionalShader,
        pointShader,
        flashShader
    );

    // BLOOM
    auto bloomRender = std::make_shared<BloomRender>(
        app, downsampling, upsampling, albedo, screenModel, 3);

    render->addRenderPass(bloomRender);

    // SCREEN

    std::vector<FrameBufferTextureCreateInfo> screenFormats = {
        TextureFormat::R8G8B8A8
    };
    auto screenFrameBuffer = std::make_shared<SimpleFrameBuffer>(
        app, screenFormats, false);
    render->addRenderPass(
        std::make_shared<DefaultRenderPassStrategy>(screenFrameBuffer));

    auto textures = fpFrameBuffer->getTextures();
    textures.push_back(albedo);
    textures.push_back(ssaoBlurFrameBuffer->getTextures().at(0)); // SSAO
    textures.push_back(bloomRender->getBloomTexture()); // BLOOM

    std::shared_ptr<Material> screenMaterial = Material::create(
        room->getApplication(), "Screen Model",
        screenFrameBuffer, screenShader,
        deferred_utils::DeferredVertex::getDescription(),
        InputDescription(0, InputRate::INSTANCE),
        {}, textures);

    screenModel->addMaterial(screenMaterial);
    screenModel->addMaterial(ssaoMaterial);
    screenModel->addMaterial(ssaoBlurMaterial);

    auto screenModelGO = room->newGameObject();
    screenModelGO->setName("Screen Model");
    screenModelGO->newComponent<GraphicComponent>(screenModel);

    auto swapFrameBuffer = std::make_shared<SwapChainFrameBuffer>(
        room, false);

    render->addRenderPass(
        std::make_shared<DefaultRenderPassStrategy>(swapFrameBuffer));

    return fpFrameBuffer;
}

void loadModels(Application* application, Room* room,
                const std::shared_ptr<FrameBuffer>& target) {
    std::shared_ptr<ShaderUniformDescriptor> materialDescriptor =
            ShaderUniformDescriptor::ofImages(application, "default", 2);

    auto shader = createShader(application,
                               "deferred", "deferred.vert", "deferred.frag");
    auto shaderParallax = createShader(application,
                                       "parallax", "deferred.vert",
                                       "deferred_parallax.frag");

    // This shader only writes on the depth buffer.
    // Allowing to precompute the depth buffer before
    // writing anything to the G-Buffer.
    auto shaderDepth = createShader(application,
                                    "depth", "deferred_depth.vert",
                                    "deferred_depth.frag");

    MaterialCreateInfo depthMaterialInfo(target, shaderDepth);
    depthMaterialInfo.descriptions.vertex.push_back(
        TestVertex::getDescription());
    depthMaterialInfo.descriptions.instance.push_back(
        DefaultInstancingData::getInstancingDescription());
    depthMaterialInfo.depthStencil.depthCompareOperation =
            DepthCompareOperation::LESS;

    auto depthMaterial = std::make_shared<Material>(
        application, "depth", depthMaterialInfo);
    depthMaterial->setPriority(1);


    MaterialCreateInfo sansMaterialInfo(target, shader);
    sansMaterialInfo.descriptions.uniform = materialDescriptor;
    sansMaterialInfo.depthStencil.depthCompareOperation =
            DepthCompareOperation::LESS_OR_EQUAL;

    auto sansLoaderInfo = assimp_loader::LoaderInfo::create<TestVertex>(
        application, "Sans", sansMaterialInfo);

    auto sansResult = assimp_loader::load(R"(resource/Sans)", "Sans.obj",
                                          sansLoaderInfo);

    if (sansResult.error.has_value()) {
        std::cout << "Couldn't load Sans model!" << std::endl;
        std::cout << std::filesystem::current_path() << std::endl;
        exit(1);
    }

    auto sansModel = sansResult.model;
    sansModel->addMaterial(depthMaterial);

    constexpr int AMOUNT = 1024;
    int q = static_cast<int>(std::sqrt(AMOUNT));
    for (int i = 0; i < AMOUNT; i++) {
        auto sans = room->newGameObject();
        sans->newComponent<GraphicComponent>(sansModel);
        sans->newComponent<ConstantRotationComponent>();

        if (i == 0) {
            auto sans2 = room->newGameObject();
            sans2->newComponent<GraphicComponent>(sansModel);
            sans2->newComponent<ConstantRotationComponent>();
            sans2->setParent(sans);
            sans2->getTransform().setPosition(rush::Vec3f(-5.0f, 5.0f, 0.0f));
            sans2->setName("Children Sans");
        }

        float x = static_cast<float>(i % q) * 3.0f;
        float z = static_cast<float>(i / q) *
                  3.0f; // NOLINT(bugprone-integer-division)
        sans->getTransform().setPosition(rush::Vec3f(x, 0.0f, z));
        sans->setName("Sans " + std::to_string(i));
    }

    auto zeppeliLoaderInfo = assimp_loader::LoaderInfo::create<TestVertex>(
        application, "Zeppeli", sansMaterialInfo);
    zeppeliLoaderInfo.flipWindingOrder = true;
    zeppeliLoaderInfo.flipNormals = true;

    auto zeppeliResult = assimp_loader::load(R"(resource/Zeppeli)",
                                             "William.obj", zeppeliLoaderInfo);

    if (zeppeliResult.error.has_value()) {
        std::cout << "Couldn't load zeppeli model!" << std::endl;
        std::cout << std::filesystem::current_path() << std::endl;
        exit(1);
    }

    auto zeppeliModel = zeppeliResult.model;
    zeppeliModel->addMaterial(depthMaterial);

    auto zeppeli = room->newGameObject();
    zeppeli->newComponent<GraphicComponent>(zeppeliModel);
    zeppeli->newComponent<ConstantRotationComponent>();
    zeppeli->getTransform().setPosition(rush::Vec3f(-10.0f, 10.0f, -10.0f));
    zeppeli->setName("Zeppeli");

    // CUBE

    TextureCreateInfo albedoInfo;
    albedoInfo.image.format = TextureFormat::R8G8B8A8_SRGB;

    std::shared_ptr<Texture> cubeAlbedo = Texture::createTextureFromFile(
        application, "cube_albedo", "resource/Cube/bricks.png",
        albedoInfo);

    std::shared_ptr<Texture> cubeNormal = Texture::createTextureFromFile(
        application, "cube_normal",
        "resource/Cube/bricks_normal.png");
    std::shared_ptr<Texture> cubeParallax = Texture::createTextureFromFile(
        application, "cube_parallax",
        "resource/Cube/bricks_parallax.png");

    std::vector<ShaderUniformBinding> cubeMaterialBindings;
    cubeMaterialBindings.resize(3, {UniformBindingType::IMAGE, 0});

    std::shared_ptr<ShaderUniformDescriptor> cubeMaterialDescriptor;
    materialDescriptor = std::make_shared<ShaderUniformDescriptor>(
        room->getApplication(),
        "cubeMaterialDescriptor",
        cubeMaterialBindings
    );

    MaterialCreateInfo cubeMaterialInfo(target, shaderParallax);
    cubeMaterialInfo.descriptions.uniform = materialDescriptor;
    cubeMaterialInfo.descriptions.vertex.
            push_back(TestVertex::getDescription());
    cubeMaterialInfo.descriptions.instance.push_back(
        DefaultInstancingData::getInstancingDescription());

    auto material = std::make_shared<Material>(
        application, "cubeMaterial", cubeMaterialInfo);

    material->getUniformBuffer()->setTexture(0, cubeAlbedo);
    material->getUniformBuffer()->setTexture(1, cubeNormal);
    material->getUniformBuffer()->setTexture(2, cubeParallax);

    auto cubeModel = model_utils::createCubeModel<TestVertex>(
        room, material);

    auto cube = room->newGameObject();
    cube->newComponent<GraphicComponent>(cubeModel);
    cube->newComponent<ConstantRotationComponent>();
    cube->getTransform().setPosition(rush::Vec3f(0.0f, 10.0f, -10.0f));
    cube->setName("Cube");
}

std::shared_ptr<Texture> loadSkybox(Room* room) {
    static const std::vector<std::string> PATHS = {
        "resource/Skybox/right.jpg",
        "resource/Skybox/left.jpg",
        "resource/Skybox/top.jpg",
        "resource/Skybox/bottom.jpg",
        "resource/Skybox/front.jpg",
        "resource/Skybox/back.jpg",
    };

    TextureCreateInfo info;
    info.imageView.viewType = TextureViewType::CUBE;
    info.image.layers = 6;
    info.image.mipmaps = 10;

    return Texture::createTextureFromFiles(room->getApplication(),
                                           "skybox", PATHS, info);
}

std::shared_ptr<Texture> loadBRDF(Room* room) {
    TextureCreateInfo info;
    info.imageView.viewType = TextureViewType::NORMAL_2D;
    info.image.layers = 1;
    info.image.mipmaps = 0;

    return Texture::createTextureFromFile(
        room->getApplication(),
        "BRDF",
        "resource/BRDF.png",
        info
    );
}

std::shared_ptr<Room> getTestRoom(Application* application) {
    auto screenModel = deferred_utils::createScreenModel(
        application,
        ModelCreateInfo(),
        "Screen Model");

    auto room = std::make_shared<Room>(application);

    auto fpFrameBuffer = initRender(room.get(), screenModel);

    auto skybox = loadSkybox(room.get());
    auto brdf = loadBRDF(room.get());
    auto& globalBuffer = application->getRender()->getGlobalUniformBuffer();
    globalBuffer.setTexture(2, skybox);
    globalBuffer.setTexture(3, computeIrradiance(application,
                                                 screenModel, skybox));
    globalBuffer.setTexture(4, brdf);


    auto cameraController = room->newGameObject();
    auto cameraMovement = cameraController->newComponent<
        CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto parameterUpdater = room->newGameObject();
    parameterUpdater->newComponent<LockMouseComponent>(cameraMovement);
    parameterUpdater->newComponent<DockSpaceComponent>();
    parameterUpdater->newComponent<ViewportComponent>();
    auto goExplorer = parameterUpdater->newComponent<
        GameObjectExplorerComponent>();
    parameterUpdater->newComponent<SceneTreeComponent>(goExplorer);
    parameterUpdater->newComponent<DebugOverlayComponent>(false, 100);

    auto globalParameters = room->newGameObject();
    globalParameters->newComponent<GlobalParametersUpdaterComponent>();
    globalParameters->setName("Global parameters");

    auto directionalLight = room->newGameObject();
    directionalLight->newComponent<DirectionalLight>();
    directionalLight->getTransform().lookAt(rush::Vec3f(0.45f, -0.6f, 0.65f));
    directionalLight->setName("Directional light");

    auto pointLightGO = room->newGameObject();
    auto pointLight = pointLightGO->newComponent<PointLight>();
    pointLightGO->getTransform().setPosition({5.0f, 7.0f, 5.0f});
    pointLightGO->setName("Point light");
    pointLight->setDiffuseColor({1.0f, 0.0f, 0.0f});
    pointLight->setConstantAttenuation(0.01f);
    pointLight->setLinearAttenuation(0.2);
    pointLight->setQuadraticAttenuation(0.1);
    pointLight->setRadiance(100.0f);

    auto flashLightGO = room->newGameObject();
    auto flashLight = flashLightGO->newComponent<FlashLight>();
    flashLightGO->getTransform().setPosition({10.0f, 7.0f, 10.0f});
    flashLightGO->getTransform().rotate(rush::Vec3f(1.0f, 0.0f, 0.0f), 1.0f);
    flashLightGO->setName("Flash light");
    flashLight->setDiffuseColor({0.0f, 1.0f, 0.0f});
    flashLight->setConstantAttenuation(0.01f);
    flashLight->setLinearAttenuation(0.2);
    flashLight->setQuadraticAttenuation(0.1);
    flashLight->setRadiance(100.0f);


    loadModels(application, room.get(), fpFrameBuffer);

    return room;
}

int main() {
    std::srand(std::time(nullptr));

    Application application(std::make_unique<vulkan::VKApplication>(
        "Neon", WIDTH, HEIGHT));

    application.init();
    application.setRoom(getTestRoom(&application));

    auto loopResult = application.startGameLoop();
    if (loopResult.isOk()) {
        std::cout << "[APPLICATION]\tApplication closed. "
                << loopResult.getResult() << " frames generated."
                << std::endl;
    }
    else {
        std::cout << "[APPLICATION]\tUnexpected game loop error: "
                << loopResult.getError()
                << std::endl;
    }

    application.setRoom(nullptr);

    return EXIT_SUCCESS;
}
