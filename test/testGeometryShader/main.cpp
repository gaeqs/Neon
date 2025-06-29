#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>
#include <random>

#include <neon/Neon.h>
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
#include "neon/render/shader/Material.h"
#include "neon/render/shader/MaterialCreateInfo.h"
#include "neon/render/shader/ShaderUniformBinding.h"

#ifdef USE_VULKAN
    #include <vulkan/util/component/VulkanInfoCompontent.h>
#endif

constexpr float WIDTH = 800;
constexpr float HEIGHT = 600;

CMRC_DECLARE(shaders);

using namespace neon;

std::vector<rush::Vec3f> randomPoints(size_t amount)
{
    std::vector<rush::Vec3f> vec;
    vec.reserve(amount);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distr(-10.0, 10.0);

    for (size_t i = 0; i < amount; ++i) {
        vec.emplace_back(distr(gen), distr(gen), distr(gen));
    }

    return vec;
}

/**
 * Creates a shader program.
 * @param room the application.
 * @param name the name of the shader.
 * @param vert the vertex shader.
 * @param frag the fragment shader.
 * @return the shader program.
 */
std::shared_ptr<ShaderProgram> createShader(Application* application, const std::string& name, const std::string& vert,
                                            const std::string& frag, const std::string& geom = "")
{
    auto shader = std::make_shared<ShaderProgram>(application, std::move(name));

    auto defaultVert = cmrc::shaders::get_filesystem().open(vert);
    auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

    shader->addShader(ShaderType::VERTEX, std::string(defaultVert.begin(), defaultVert.end()));

    shader->addShader(ShaderType::FRAGMENT, std::string(defaultFrag.begin(), defaultFrag.end()));

    if (!geom.empty()) {
        auto defaultGeom = cmrc::shaders::get_filesystem().open(geom);

        shader->addShader(ShaderType::GEOMETRY, std::string(defaultGeom.begin(), defaultGeom.end()));
    }

    auto result = shader->compile();
    if (result.has_value()) {
        error() << result.value();
        throw std::runtime_error(result.value());
    }

    return shader;
}

std::shared_ptr<FrameBuffer> initRender(Room* room)
{
    auto* app = room->getApplication();

    // Bindings for the global uniforms.
    // In this application, we have a buffer of global parameters
    // and a skybox.
    std::vector<ShaderUniformBinding> globalBindings = {ShaderUniformBinding::uniformBuffer(sizeof(Matrices)),
                                                        ShaderUniformBinding::image()};

    // The description of the global uniforms.
    auto globalDescriptor = std::make_shared<ShaderUniformDescriptor>(app, "default", globalBindings);

    // The render of the application.
    // We should set the render to the application before
    // we do anything else, as some components depend on
    // the application's render.
    auto render = std::make_shared<Render>(app, "default", globalDescriptor);
    app->setRender(render);

    auto screenShader = createShader(app, "screen", "screen.vert", "screen.frag");

    std::vector<FrameBufferTextureCreateInfo> frameBufferFormats = {TextureFormat::R8G8B8A8};

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(app, "frame_buffer", SamplesPerTexel::COUNT_1,
                                                             frameBufferFormats, FrameBufferDepthCreateInfo());
    app->getAssets().store(fpFrameBuffer, AssetStorageMode::PERMANENT);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("frame_buffer", fpFrameBuffer));

    std::vector<FrameBufferTextureCreateInfo> screenFormats = {TextureFormat::R8G8B8A8};
    auto screenFrameBuffer =
        std::make_shared<SimpleFrameBuffer>(app, "screen", SamplesPerTexel::COUNT_1, screenFormats);
    app->getAssets().store(screenFrameBuffer, AssetStorageMode::PERMANENT);
    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("screen", screenFrameBuffer));

    auto outputs = fpFrameBuffer->getOutputs();
    std::vector<std::shared_ptr<SampledTexture>> textures;
    textures.reserve(outputs.size());

    for (auto& output : outputs) {
        textures.push_back(SampledTexture::create(app, output.resolvedTexture));
    }

    std::shared_ptr screenMaterial = Material::create(room->getApplication(), "Screen Model", screenFrameBuffer,
                                                      screenShader, deferred_utils::DeferredVertex::getDescription(),
                                                      InputDescription(0, InputRate::INSTANCE), {}, textures);

    auto screenModel = deferred_utils::createScreenModel(room->getApplication(), ModelCreateInfo(), "Screen Model");

    screenModel->addMaterial(screenMaterial);

    auto screenModelGO = room->newGameObject();
    screenModelGO->setName("Screen Model");
    screenModelGO->newComponent<GraphicComponent>(screenModel);

    auto swapFrameBuffer = std::make_shared<SwapChainFrameBuffer>(app, "swap_chain", SamplesPerTexel::COUNT_1, false);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("swap_chain", swapFrameBuffer));

    return fpFrameBuffer;
}

void loadModels(Application* application, Room* room, const std::shared_ptr<FrameBuffer>& target)
{
    std::shared_ptr materialDescriptor = ShaderUniformDescriptor::ofImages(application, "default", 2);

    auto shader = createShader(application, "deferred", "deferred.vert", "deferred.frag", "deferred.geom");

    // CUBE
    std::vector<ShaderUniformBinding> cubeMaterialBindings;

    std::shared_ptr<ShaderUniformDescriptor> cubeMaterialDescriptor;
    materialDescriptor = std::make_shared<ShaderUniformDescriptor>(room->getApplication(), "pointMaterialDescriptor",
                                                                   cubeMaterialBindings);

    MaterialCreateInfo cubeMaterialInfo(target, shader);
    cubeMaterialInfo.descriptions.uniform = materialDescriptor;
    cubeMaterialInfo.descriptions.vertex.push_back(TestVertex::getDescription());
    cubeMaterialInfo.descriptions.instance.push_back(DefaultInstancingData::getInstancingDescription());
    cubeMaterialInfo.topology = PrimitiveTopology::POINT_LIST;
    cubeMaterialInfo.rasterizer.polygonMode = PolygonMode::LINE;
    cubeMaterialInfo.rasterizer.cullMode = CullMode::NONE;

    auto material = std::make_shared<Material>(application, "pointMaterial", cubeMaterialInfo);

    auto model = model_utils::createModel<rush::Vec3f>(room, "points", material, randomPoints(10000));

    auto object = room->newGameObject();
    object->newComponent<GraphicComponent>(model);
    object->getTransform().setPosition(rush::Vec3f(0.0f, 10.0f, -10.0f));
    object->setName("Object");
}

std::shared_ptr<SampledTexture> loadSkybox(Room* room)
{
    static const std::vector<std::string> PATHS = {
        "resource/Skybox/right.jpg",  "resource/Skybox/left.jpg",  "resource/Skybox/top.jpg",
        "resource/Skybox/bottom.jpg", "resource/Skybox/front.jpg", "resource/Skybox/back.jpg",
    };

    TextureCreateInfo info;
    info.image.viewType = TextureViewType::CUBE;
    info.imageView.viewType = TextureViewType::CUBE;
    info.image.layers = 6;
    info.image.mipmaps = 10;

    auto texture = Texture::createTextureFromFiles(room->getApplication(), "skybox", PATHS, info.image);
    auto view = TextureView::create(room->getApplication(), "skybox", info.imageView, texture);
    return SampledTexture::create(room->getApplication(), "skybox", std::make_shared<MutableAsset<TextureView>>(view));
}


std::shared_ptr<Room> getTestRoom(Application* application)
{
    auto room = std::make_shared<Room>(application);

    auto fpFrameBuffer = initRender(room.get());
    auto screenFrameBuffer = application->getAssets().get<FrameBuffer>("screen").value();

    auto skybox = loadSkybox(room.get());
    application->getRender()->getGlobalUniformBuffer().setTexture(1, skybox);

    auto cameraController = room->newGameObject();
    auto cameraMovement = cameraController->newComponent<CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto parameterUpdater = room->newGameObject();
    parameterUpdater->newComponent<GlobalParametersUpdaterComponent>();
    parameterUpdater->newComponent<LockMouseComponent>(cameraMovement);
    parameterUpdater->newComponent<DockSpaceComponent>();
    parameterUpdater->newComponent<ViewportComponent>(std::dynamic_pointer_cast<SimpleFrameBuffer>(screenFrameBuffer));
    auto goExplorer = parameterUpdater->newComponent<GameObjectExplorerComponent>();
    parameterUpdater->newComponent<SceneTreeComponent>(goExplorer);
    parameterUpdater->newComponent<DebugOverlayComponent>(false, 100);
    parameterUpdater->newComponent<LogComponent>();

#ifdef USE_VULKAN
    parameterUpdater->newComponent<vulkan::VulkanInfoCompontent>();
#endif

    loadModels(application, room.get(), fpFrameBuffer);

    return room;
}

int main()
{
    std::srand(std::time(nullptr));

    vulkan::VKApplicationCreateInfo info;
    info.name = "Neon";
    info.windowSize = {WIDTH, HEIGHT};
    info.vSync = false;
    info.defaultExtensionInclusion = vulkan::InclusionMode::EXCLUDE_ALL;
    info.defaultFeatureInclusion = vulkan::InclusionMode::EXCLUDE_ALL;
    info.featuresConfigurator = [](const auto& device, vulkan::VKPhysicalDeviceFeatures& features) {
        vulkan::VKApplicationCreateInfo::defaultFeaturesConfigurer(device, features);
        features.basicFeatures.geometryShader = true;
        features.basicFeatures.fillModeNonSolid = true;
        features.basicFeatures.samplerAnisotropy = true;
    };

    info.extraFeatures.emplace_back(
        VkPhysicalDeviceMeshShaderFeaturesEXT(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT));

    info.enableValidationLayers = true;

    Application application(std::make_unique<vulkan::VKApplication>(info));

    application.init();
    application.setRoom(getTestRoom(&application));

    auto loopResult = application.startGameLoop();
    if (loopResult.isOk()) {
        logger.done(MessageBuilder()
                        .print("Application closed. ")
                        .print(loopResult.getResult(), TextEffect::foreground4bits(2))
                        .print(" frames generated."));
    } else {
        logger.done(MessageBuilder()
                        .print("Unexpected game loop error: ")
                        .print(loopResult.getError(), TextEffect::foreground4bits(1)));
    }

    application.setRoom(nullptr);

    return EXIT_SUCCESS;
}
