#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>

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
#include <neon/version.h>
#include <vulkan/util/component/VulkanInfoCompontent.h>

#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "ConstantRotationComponent.h"
#include "neon/render/shader/Material.h"
#include "neon/render/shader/MaterialCreateInfo.h"
#include "neon/render/shader/ShaderUniformBinding.h"

constexpr float WIDTH = 800;
constexpr float HEIGHT = 600;

CMRC_DECLARE(resources);

using namespace neon;

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
    auto defaultVert = cmrc::resources::get_filesystem().open(vert);
    auto defaultFrag = cmrc::resources::get_filesystem().open(frag);

    auto result = ShaderProgram::createShader(application, name, defaultVert, defaultFrag);
    if (!result.isOk()) {
        error() << result.getError();
        throw std::runtime_error(result.getError());
    }

    return result.getResult();
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

    auto directionalShader = createShader(app, "directional_light", "directional_light.vert", "directional_light.frag");
    auto pointShader = createShader(app, "point_light", "point_light.vert", "point_light.frag");
    auto flashShader = createShader(app, "flash_light", "flash_light.vert", "flash_light.frag");
    auto screenShader = createShader(app, "screen", "screen.vert", "screen.frag");

    std::vector<FrameBufferTextureCreateInfo> frameBufferFormats = {
        TextureFormat::R8G8B8A8,
        TextureFormat::R16FG16F, // NORMAL XY
        TextureFormat::R16FG16F  // NORMAL Z / SPECULAR
    };

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(app, "frame_buffer", frameBufferFormats, true);
    app->getAssets().store(fpFrameBuffer, AssetStorageMode::PERMANENT);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("frame_buffer", fpFrameBuffer));

    auto outputs = fpFrameBuffer->getOutputs();
    std::vector<std::shared_ptr<Texture>> textures;
    textures.reserve(outputs.size());

    for (auto& output : outputs) {
        textures.push_back(output.resolvedTexture);
    }

    auto albedo = deferred_utils::createLightSystem(room, render.get(), textures, TextureFormat::R8G8B8A8,
                                                    directionalShader, pointShader, flashShader);

    std::vector<FrameBufferTextureCreateInfo> screenFormats = {TextureFormat::R8G8B8A8};
    auto screenFrameBuffer = std::make_shared<SimpleFrameBuffer>(app, "screen", screenFormats, false);
    app->getAssets().store(screenFrameBuffer, AssetStorageMode::PERMANENT);
    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("screen", screenFrameBuffer));

    textures[0] = albedo;

    std::shared_ptr screenMaterial = Material::create(room->getApplication(), "Screen Model", screenFrameBuffer,
                                                      screenShader, deferred_utils::DeferredVertex::getDescription(),
                                                      InputDescription(0, InputRate::INSTANCE), {}, textures);

    auto screenModel = deferred_utils::createScreenModel(room->getApplication(), ModelCreateInfo(), "Screen Model");

    screenModel->addMaterial(screenMaterial);

    auto screenModelGO = room->newGameObject();
    screenModelGO->setName("Screen Model");
    screenModelGO->newComponent<GraphicComponent>(screenModel);

    auto swapFrameBuffer =
        std::make_shared<SwapChainFrameBuffer>(app, "swap_chain_frame_buffer", SamplesPerTexel::COUNT_1, false);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("swap_chain", swapFrameBuffer));

    return fpFrameBuffer;
}

void sansLoadThread(Application* application)
{
    DEBUG_PROFILE_ID(application->getProfiler(), sans, "Sans load thread");
    auto holder = application->getCommandManager().fetchCommandPool();
    auto buffer = holder.getPool().beginCommandBuffer(true);

    DirectoryFileSystem fileSystem("resource");
    AssetLoaderContext context(application);
    context.fileSystem = &fileSystem;
    context.commandBuffer = buffer;
    auto sansModel = loadAssetFromFile<Model>("sans_model.json", context);

    if (sansModel == nullptr) {
        error() << "Couldn't load Sans model! ";
        exit(1);
    }

    buffer->end();
    buffer->submit();
    buffer->wait();

    done() << "SANS UPLOADED!";

    application->getTaskRunner().executeOnMainThread([application, sansModel] {
        constexpr int AMOUNT = 1024 * 1;

        auto room = application->getRoom();

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
            sans->getTransform().setPosition(rush::Vec3f(x, 0, z));
            sans->setName("Sans " + std::to_string(i));
        }
    });
}

void loadModels(Application* application, Room* room, const std::shared_ptr<FrameBuffer>& target)
{
    std::shared_ptr<ShaderUniformDescriptor> materialDescriptor =
        ShaderUniformDescriptor::ofImages(application, "default", 2);

    application->getAssets().store(materialDescriptor, AssetStorageMode::PERMANENT);

    auto shader = createShader(application, "deferred", "deferred.vert", "deferred.frag");
    auto shaderParallax = createShader(application, "parallax", "deferred.vert", "deferred_parallax.frag");
    application->getAssets().store(shader, AssetStorageMode::PERMANENT);

    MaterialCreateInfo sansMaterialInfo(target, shader);
    sansMaterialInfo.descriptions.uniform = materialDescriptor;

    auto sansLoaderInfo = assimp_loader::LoaderInfo::create<TestVertex>(application, "Sans", sansMaterialInfo);

    std::function func = sansLoadThread;

    auto task = application->getTaskRunner().executeAsync(func, application);

    auto zeppeliLoaderInfo = assimp_loader::LoaderInfo::create<TestVertex>(application, "Zeppeli", sansMaterialInfo);
    zeppeliLoaderInfo.flipWindingOrder = true;
    zeppeliLoaderInfo.flipNormals = true;

    auto zeppeliResult = assimp_loader::load(R"(resource/Zeppeli)", "William.obj", zeppeliLoaderInfo);

    if (zeppeliResult.error.has_value()) {
        error() << "Couldn't load model!";
        exit(1);
    }

    auto zeppeliModel = zeppeliResult.model;

    auto zeppeli = room->newGameObject();
    zeppeli->newComponent<GraphicComponent>(zeppeliModel);
    zeppeli->newComponent<ConstantRotationComponent>();
    zeppeli->getTransform().setPosition(rush::Vec3f(-10.0f, 10.0f, -10.0f));
    zeppeli->setName("Zeppeli");

    // CUBE

    TextureCreateInfo albedoInfo;
    albedoInfo.image.format = TextureFormat::R8G8B8A8_SRGB;

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

    material->getUniformBuffer()->setTexture(0, cubeAlbedo);
    material->getUniformBuffer()->setTexture(1, cubeNormal);
    material->getUniformBuffer()->setTexture(2, cubeParallax);

    auto cubeModel = model_utils::createCubeModel<TestVertex>(room, material);

    auto cube = room->newGameObject();
    cube->newComponent<GraphicComponent>(cubeModel);
    cube->newComponent<ConstantRotationComponent>();
    cube->getTransform().setPosition(rush::Vec3f(0.0f, 10.0f, -10.0f));
    cube->setName("Cube");
}

std::shared_ptr<Texture> loadSkybox(Room* room)
{
    CMRCFileSystem fileSystem(cmrc::resources::get_filesystem());
    AssetLoaderContext context(room->getApplication(), nullptr, &fileSystem);
    return loadAssetFromFile<Texture>("texture/skybox/skybox.json", context);
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
    parameterUpdater->newComponent<vulkan::VulkanInfoCompontent>();

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

    auto flashLightGO = room->newGameObject();
    auto flashLight = flashLightGO->newComponent<FlashLight>();
    flashLightGO->getTransform().setPosition({10.0f, 7.0f, 10.0f});
    flashLightGO->getTransform().rotate(rush::Vec3f(1.0f, 0.0f, 0.0f), 1.0f);
    flashLightGO->setName("Flash light");
    flashLight->setDiffuseColor({0.0f, 1.0f, 0.0f});
    flashLight->setConstantAttenuation(0.01f);
    flashLight->setLinearAttenuation(0.2f);
    flashLight->setQuadraticAttenuation(0.1f);

    loadModels(application, room.get(), fpFrameBuffer);

    return room;
}

int main()
{

    neon::debug() << "Using Neon " << NEON_VERSION;

    std::srand(std::time(nullptr));

    vulkan::VKApplicationCreateInfo info;
    info.name = "Neon";
    info.windowSize = {WIDTH, HEIGHT};
    info.vSync = false;

    info.featuresConfigurator = [](const auto& d, auto& f) {
        vulkan::VKApplicationCreateInfo::defaultFeaturesConfigurer(d, f);
        f.basicFeatures.samplerAnisotropy = true;
        f.basicFeatures.samplerAnisotropy = true;
    };

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
