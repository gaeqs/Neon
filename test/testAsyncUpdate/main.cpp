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
#include <neon/util/component/LogComponent.h>

#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "ConstantRotationComponent.h"
#include "BloomRender.h"
#include "ModelAsyncFlushComponent.h"

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
std::shared_ptr<ShaderProgram> createShader(Application* application, const std::string& name, const std::string& vert,
                                            const std::string& frag)
{
    auto defaultVert = cmrc::shaders::get_filesystem().open(vert);
    auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

    auto result = ShaderProgram::createShader(application, name, defaultVert, defaultFrag);
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
std::shared_ptr<neon::ShaderProgram> createShader(neon::Application* application, const std::string& name,
                                                  const std::string& vert, const std::string& geom,
                                                  const std::string& frag)
{
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

std::shared_ptr<Material> createSSAOMaterial(Room* room, const std::shared_ptr<FrameBuffer>& ssaoFrameBuffer,
                                             std::vector<std::shared_ptr<SampledTexture>> textures)
{
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
        samples.push_back(rush::Vec4f(randomFloats(randomGenerator) * 2.0f - 1.0f,
                                      randomFloats(randomGenerator) * 2.0f - 1.0f, randomFloats(randomGenerator), 0.0f)
                              .normalized() *
                          std::lerp(0.1f, 1.0f, scale));
    }

    // GENERATE NOISE
    std::vector<rush::Vec3f> noise;
    noise.reserve(NOISE_WIDTH * NOISE_WIDTH);
    for (uint32_t i = 0; i < NOISE_WIDTH * NOISE_WIDTH; ++i) {
        noise.emplace_back(randomFloats(randomGenerator) * 2.0f - 1.0f, randomFloats(randomGenerator) * 2.0f - 1.0f,
                           0.0f);
    }

    TextureCreateInfo noiseTextureCreateInfo;
    noiseTextureCreateInfo.format = TextureFormat::R32FG32FB32F;
    noiseTextureCreateInfo.width = NOISE_WIDTH;
    noiseTextureCreateInfo.height = NOISE_WIDTH;
    noiseTextureCreateInfo.depth = 1;
    noiseTextureCreateInfo.mipmaps = 1;
    // RTX 2070 doesn't have support for R32FG32FB32F when using optimal tiling,
    // but it does when using linear tiling.
    noiseTextureCreateInfo.tiling = Tiling::LINEAR;

    auto noiseTexture =
        Texture::createFromRawData(room->getApplication(), "SSAO_noise", noise.data(), noiseTextureCreateInfo);
    textures.push_back(SampledTexture::create(room->getApplication(), noiseTexture));

    auto ssaoShader = createShader(room->getApplication(), "SSAO", "ssao.vert", "ssao.frag");

    std::shared_ptr<Material> material =
        Material::create(room->getApplication(), "SSAO", ssaoFrameBuffer, ssaoShader,
                         deferred_utils::DeferredVertex::getDescription(), InputDescription(0, InputRate::INSTANCE),
                         {
                             {samples.data(), SAMPLES * sizeof(rush::Vec4f)}
    },
                         textures);

    return material;
}

std::shared_ptr<Material> createSSAOBlurMaterial(Room* room, const std::shared_ptr<FrameBuffer>& ssaoFrameBuffer,
                                                 const std::vector<std::shared_ptr<MutableAsset<TextureView>>>& textures)
{
    auto ssaoShader = createShader(room->getApplication(), "SSAO", "ssao_blur.vert", "ssao_blur.frag");

    return Material::create(room->getApplication(), "SSAO Blur", ssaoFrameBuffer, ssaoShader,
                            deferred_utils::DeferredVertex::getDescription(), InputDescription(0, InputRate::INSTANCE),
                            {}, SampledTexture::create(room->getApplication(), textures));
}

std::shared_ptr<SampledTexture> computeIrradiance(Application* app, const std::shared_ptr<Model>& screenModel,
                                                  const std::shared_ptr<SampledTexture>& skybox)
{
    neon::FrameBufferTextureCreateInfo testInfo;
    testInfo.layers = 6;
    testInfo.imageView.viewType = neon::TextureViewType::CUBE;

    auto testFrameBuffer = std::make_shared<SimpleFrameBuffer>(
        app, "neoneuron:test", SamplesPerTexel::COUNT_1, std::vector{testInfo},
        std::optional<FrameBufferDepthCreateInfo>(), [](const auto& _) { return false; },
        [](const auto& _) { return rush::Vec2ui(1024, 1024); });

    auto irradianceShader = createShader(app, "Irradiance", "irradiance_map_creation.vert",
                                         "irradiance_map_creation.geom", "irradiance_map_creation.frag");

    // This material should not be destroyed yet.material
    // We have to wait for the component to be destroyed!
    auto material = neon::Material::create(app, "Irradiance", testFrameBuffer, irradianceShader,
                                           neon::deferred_utils::DeferredVertex::getDescription(),
                                           neon::InputDescription(0, neon::InputRate::INSTANCE), {}, {skybox});

    neon::CommandBuffer cf(app, true);
    cf.begin(true);

    material->getUniformBuffer()->prepareForFrame(&cf);

    auto& gub = app->getRender()->getGlobalUniformBuffer();
    gub.prepareForFrame(&cf);

    screenModel->drawOutside(material.get(), &cf);

    cf.end();
    cf.submit();

    // We have to set the texture on the next frame.
    return SampledTexture::create(app, testFrameBuffer->getOutputs()[0].resolvedTexture);
}

std::shared_ptr<FrameBuffer> initRender(Room* room, const std::shared_ptr<Model>& screenModel)
{
    auto* app = room->getApplication();

    // Bindings for the global uniforms.
    // In this application, we have a buffer of global parameters
    // and two textures: a skybox and an irradiance skybox.
    std::vector<ShaderUniformBinding> globalBindings = {ShaderUniformBinding::uniformBuffer(sizeof(Matrices)),
                                                        ShaderUniformBinding::uniformBuffer(sizeof(PBRParameters)),
                                                        ShaderUniformBinding::image(), ShaderUniformBinding::image(),
                                                        ShaderUniformBinding::image()};

    // The description of the global uniforms.
    auto globalDescriptor = std::make_shared<ShaderUniformDescriptor>(app, "default", globalBindings);

    // The render of the application.
    // We should set the render to the application before
    // we do anything else, as some components depend on
    // the application's render.
    auto render = std::make_shared<Render>(app, "default", globalDescriptor);
    app->setRender(render);

    auto directionalShader = createShader(app, "directional_light", "directional_light.vert", "directional_light.frag");
    auto pointShader = createShader(app, "point_light", "point_light.vert", "point_light.frag");
    auto flashShader = createShader(app, "flash_light", "flash_light.vert", "flash_light.frag");
    auto screenShader = createShader(app, "screen", "screen.vert", "screen.frag");
    auto downsampling = createShader(app, "downsampling", "downsampling.vert", "downsampling.frag");
    auto upsampling = createShader(app, "upsampling", "upsampling.vert", "upsampling.frag");

    // G BUFFER

    std::vector<FrameBufferTextureCreateInfo> frameBufferFormats = {
        TextureFormat::R16FG16FB16FA16F, // ALBEDO
        TextureFormat::R16FG16F,         // NORMAL XY
        TextureFormat::R16FG16F          // METALLIC / ROUGHNESS
    };

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(app, "frame_buffer", SamplesPerTexel::COUNT_1,
                                                             frameBufferFormats, FrameBufferDepthCreateInfo());
    app->getAssets().store(fpFrameBuffer, AssetStorageMode::PERMANENT);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("frame_buffer", fpFrameBuffer));

    // Out textures. You can use these in other frame buffers.
    auto fpTextures = SampledTexture::create(app, fpFrameBuffer->getTextures());

    // SSAO

    std::vector<FrameBufferTextureCreateInfo> ssaoTextureInfo{TextureFormat::R32F};
    ssaoTextureInfo[0].sampler.anisotropy = false;
    ssaoTextureInfo[0].sampler.magnificationFilter = TextureFilter::LINEAR;
    ssaoTextureInfo[0].sampler.minificationFilter = TextureFilter::LINEAR;

    auto ssaoFrameBuffer = std::make_shared<SimpleFrameBuffer>(app, "ssao", SamplesPerTexel::COUNT_1, ssaoTextureInfo,
                                                               std::optional<FrameBufferDepthCreateInfo>());

    app->getRender()->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("ssao_frame_buffer", ssaoFrameBuffer));

    auto ssaoBlurFrameBuffer =
        std::make_shared<SimpleFrameBuffer>(app, "ssao_blur", SamplesPerTexel::COUNT_1, ssaoTextureInfo);

    app->getRender()->addRenderPass(
        std::make_shared<DefaultRenderPassStrategy>("ssao_blur_buffer", ssaoBlurFrameBuffer));

    auto ssaoMaterial = createSSAOMaterial(room, ssaoFrameBuffer,
                                           {
                                               fpTextures.at(0), // ALBEDO
                                               fpTextures.at(1), // NORMAL XY
                                               fpTextures.at(3)  // DEPTH
                                           });

    auto ssaoBlurMaterial = createSSAOBlurMaterial(room, ssaoBlurFrameBuffer, ssaoFrameBuffer->getTextures());

    // LIGHT SYSTEM

    auto albedo = SampledTexture::create(
        app, deferred_utils::createLightSystem(room, render.get(), fpTextures, TextureFormat::R16FG16FB16FA16F,
                                               directionalShader, pointShader, flashShader));

    // BLOOM
    auto bloomRender = std::make_shared<BloomRender>(app, downsampling, upsampling, albedo, screenModel, 3);

    render->addRenderPass(bloomRender);

    // SCREEN

    std::vector<FrameBufferTextureCreateInfo> screenFormats = {TextureFormat::R8G8B8A8};
    auto screenFrameBuffer =
        std::make_shared<SimpleFrameBuffer>(app, "screen", SamplesPerTexel::COUNT_1, screenFormats);
    app->getAssets().store(screenFrameBuffer, AssetStorageMode::PERMANENT);
    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("screen", screenFrameBuffer));

    auto textures = SampledTexture::create(app, fpFrameBuffer->getTextures());
    textures.push_back(albedo);
    textures.push_back(SampledTexture::create(app, ssaoBlurFrameBuffer->getTextures().at(0))); // SSAO
    textures.push_back(SampledTexture::create(app, bloomRender->getBloomTexture()));           // BLOOM

    std::shared_ptr<Material> screenMaterial = Material::create(
        room->getApplication(), "Screen Model", screenFrameBuffer, screenShader,
        deferred_utils::DeferredVertex::getDescription(), InputDescription(0, InputRate::INSTANCE), {}, textures);

    screenModel->addMaterial(screenMaterial);
    screenModel->addMaterial(ssaoMaterial);
    screenModel->addMaterial(ssaoBlurMaterial);

    auto screenModelGO = room->newGameObject();
    screenModelGO->setName("Screen Model");
    screenModelGO->newComponent<GraphicComponent>(screenModel);

    auto swapFrameBuffer = std::make_shared<SwapChainFrameBuffer>(app, "swap_chain", SamplesPerTexel::COUNT_1, false);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("swap_chain", swapFrameBuffer));

    return fpFrameBuffer;
}

void loadModels(Application* application, Room* room, const std::shared_ptr<FrameBuffer>& target)
{
    std::shared_ptr<ShaderUniformDescriptor> materialDescriptor =
        ShaderUniformDescriptor::ofImages(application, "default", 2);

    auto shader = createShader(application, "deferred", "deferred.vert", "deferred.frag");
    auto shaderParallax = createShader(application, "parallax", "deferred.vert", "deferred_parallax.frag");

    // This shader only writes on the depth buffer.
    // Allowing to precompute the depth buffer before
    // writing anything to the G-Buffer.
    auto shaderDepth = createShader(application, "depth", "deferred_depth.vert", "deferred_depth.frag");

    MaterialCreateInfo depthMaterialInfo(target, shaderDepth);
    depthMaterialInfo.descriptions.vertex.push_back(TestVertex::getDescription());
    depthMaterialInfo.descriptions.instance.push_back(DefaultInstancingData::getInstancingDescription());
    depthMaterialInfo.depthStencil.depthCompareOperation = DepthCompareOperation::LESS;

    auto depthMaterial = std::make_shared<Material>(application, "depth", depthMaterialInfo);
    depthMaterial->setPriority(1);

    MaterialCreateInfo sansMaterialInfo(target, shader);
    sansMaterialInfo.descriptions.uniform = materialDescriptor;
    sansMaterialInfo.depthStencil.depthCompareOperation = DepthCompareOperation::LESS_OR_EQUAL;

    auto sansLoaderInfo = assimp_loader::LoaderInfo::create<TestVertex>(application, "Sans", sansMaterialInfo);

    sansLoaderInfo.defineInstanceProvider<ConcurrentInstanceData>();

    auto sansResult = assimp_loader::load(R"(resource/Sans)", "Sans.obj", sansLoaderInfo);

    if (sansResult.error.has_value()) {
        std::cout << "Couldn't load Sans model!" << std::endl;
        std::cout << std::filesystem::current_path() << std::endl;
        exit(1);
    }

    auto sansModel = sansResult.model;
    sansModel->addMaterial(depthMaterial);
    sansModel->setShouldAutoFlush(false);

    application->getAssets().store(sansModel, AssetStorageMode::PERMANENT);

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
        float z = static_cast<float>(i / q) * 3.0f; // NOLINT(bugprone-integer-division)
        sans->getTransform().setPosition(rush::Vec3f(x, 0.0f, z));
        sans->setName("Sans " + std::to_string(i));
    }

    auto zeppeliLoaderInfo = assimp_loader::LoaderInfo::create<TestVertex>(application, "Zeppeli", sansMaterialInfo);
    zeppeliLoaderInfo.flipWindingOrder = true;
    zeppeliLoaderInfo.flipNormals = true;

    auto zeppeliResult = assimp_loader::load(R"(resource/Zeppeli)", "William.obj", zeppeliLoaderInfo);

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
    albedoInfo.format = TextureFormat::R8G8B8A8_SRGB;

    std::shared_ptr<Texture> cubeAlbedo =
        Texture::createTextureFromFile(application, "cube_albedo", "resource/Cube/bricks.png", albedoInfo);

    std::shared_ptr<Texture> cubeNormal =
        Texture::createTextureFromFile(application, "cube_normal", "resource/Cube/bricks_normal.png");
    std::shared_ptr<Texture> cubeParallax =
        Texture::createTextureFromFile(application, "cube_parallax", "resource/Cube/bricks_parallax.png");

    std::vector<ShaderUniformBinding> cubeMaterialBindings;
    cubeMaterialBindings.resize(3, ShaderUniformBinding::image());

    std::shared_ptr<ShaderUniformDescriptor> cubeMaterialDescriptor;
    materialDescriptor = std::make_shared<ShaderUniformDescriptor>(room->getApplication(), "cubeMaterialDescriptor",
                                                                   cubeMaterialBindings);

    MaterialCreateInfo cubeMaterialInfo(target, shaderParallax);
    cubeMaterialInfo.descriptions.uniform = materialDescriptor;
    cubeMaterialInfo.descriptions.vertex.push_back(TestVertex::getDescription());
    cubeMaterialInfo.descriptions.instance.push_back(DefaultInstancingData::getInstancingDescription());

    auto material = std::make_shared<Material>(application, "cubeMaterial", cubeMaterialInfo);

    material->getUniformBuffer()->setTexture(0, SampledTexture::create(application, cubeAlbedo));
    material->getUniformBuffer()->setTexture(1, SampledTexture::create(application, cubeNormal));
    material->getUniformBuffer()->setTexture(2, SampledTexture::create(application, cubeParallax));

    auto cubeModel = model_utils::createCubeModel<TestVertex>(room, material);

    auto cube = room->newGameObject();
    cube->newComponent<GraphicComponent>(cubeModel);
    cube->newComponent<ConstantRotationComponent>();
    cube->getTransform().setPosition(rush::Vec3f(0.0f, 10.0f, -10.0f));
    cube->setName("Cube");
}

std::shared_ptr<SampledTexture> loadSkybox(Room* room)
{
    static const std::vector<std::string> PATHS = {
        "resource/Skybox/right.jpg",  "resource/Skybox/left.jpg",  "resource/Skybox/top.jpg",
        "resource/Skybox/bottom.jpg", "resource/Skybox/front.jpg", "resource/Skybox/back.jpg",
    };

    TextureCreateInfo info;
    info.imageView.viewType = TextureViewType::CUBE;
    info.image.layers = 6;
    info.image.mipmaps = 10;

    auto texture = Texture::createTextureFromFiles(room->getApplication(), "skybox", PATHS, info.image);
    auto view = TextureView::create(room->getApplication(), "skybox", info.imageView, texture);
    return SampledTexture::create(room->getApplication(), "skybox", std::make_shared<MutableAsset<TextureView>>(view));
}

std::shared_ptr<SampledTexture> loadBRDF(Room* room)
{
    TextureCreateInfo info;
    info.layers = 1;
    info.mipmaps = 0;

    auto texture = Texture::createTextureFromFile(room->getApplication(), "BRDF", "resource/BRDF.png", info);
    return SampledTexture::create(room->getApplication(), "BRDF", texture);
}

std::shared_ptr<Room> getTestRoom(Application* application)
{
    auto screenModel = deferred_utils::createScreenModel(application, ModelCreateInfo(), "Screen Model");

    auto room = std::make_shared<Room>(application);

    auto fpFrameBuffer = initRender(room.get(), screenModel);
    auto screenFrameBuffer = application->getAssets().get<FrameBuffer>("screen").value();

    auto skybox = loadSkybox(room.get());
    auto brdf = loadBRDF(room.get());
    auto& globalBuffer = application->getRender()->getGlobalUniformBuffer();
    globalBuffer.setTexture(2, skybox);
    globalBuffer.setTexture(3, computeIrradiance(application, screenModel, skybox));
    globalBuffer.setTexture(4, brdf);

    auto cameraController = room->newGameObject();
    auto cameraMovement = cameraController->newComponent<CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto parameterUpdater = room->newGameObject();
    parameterUpdater->newComponent<LockMouseComponent>(cameraMovement);
    parameterUpdater->newComponent<DockSpaceComponent>();
    parameterUpdater->newComponent<ViewportComponent>(std::dynamic_pointer_cast<SimpleFrameBuffer>(screenFrameBuffer));
    parameterUpdater->newComponent<LogComponent>();
    auto goExplorer = parameterUpdater->newComponent<GameObjectExplorerComponent>();
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

    auto sans = application->getAssets().get<Model>("Sans");

    if (sans.has_value()) {
        debug() << "Sans found in assets";
        auto updaterGO = room->newGameObject();
        updaterGO->newComponent<ModelAsyncFlushComponent>(sans.value());
    }

    return room;
}

int main()
{
    std::srand(std::time(nullptr));

    vulkan::VKApplicationCreateInfo info;
    info.name = "Neon";
    info.windowSize = {WIDTH, HEIGHT};

    info.featuresConfigurator = [](const auto& d, auto& f) {
        vulkan::VKApplicationCreateInfo::defaultFeaturesConfigurer(d, f);
        f.basicFeatures.samplerAnisotropy = true;
        f.basicFeatures.geometryShader = true;
    };

    Application application(std::make_unique<vulkan::VKApplication>(info));

    application.init();
    application.setRoom(getTestRoom(&application));

    auto loopResult = application.startGameLoop();
    if (loopResult.isOk()) {
        std::cout << "[APPLICATION]\tApplication closed. " << loopResult.getResult() << " frames generated."
                  << std::endl;
    } else {
        std::cout << "[APPLICATION]\tUnexpected game loop error: " << loopResult.getError() << std::endl;
    }

    application.setRoom(nullptr);

    return EXIT_SUCCESS;
}
