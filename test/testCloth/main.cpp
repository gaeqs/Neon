#include <iostream>
#include <memory>
#include <vector>

#include <neon/Neon.h>

#include <neon/util/component/CameraMovementComponent.h>
#include <neon/util/component/DebugOverlayComponent.h>
#include <neon/util/component/DockSpaceComponent.h>
#include <neon/util/component/ViewportComponent.h>
#include <neon/util/component/SceneTreeComponent.h>
#include <neon/util/component/GameObjectExplorerComponent.h>
#include <neon/util/DeferredUtils.h>

#include <neon/assimp/AssimpLoader.h>

#include "Cloth.h"
#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "cloth/PhysicsManager.h"

constexpr int32_t WIDTH = 800;
constexpr int32_t HEIGHT = 600;

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

    auto shader = std::make_shared<ShaderProgram>(application, name);
    shader->addShader(ShaderType::VERTEX, defaultVert);
    shader->addShader(ShaderType::FRAGMENT, defaultFrag);

    auto result = shader->compile();
    if (result.has_value()) {
        std::cerr << result.value() << std::endl;
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

    auto directionalShader = createShader(app, "directional_light", "directional_light.vert", "directional_light.frag");
    auto pointShader = createShader(app, "point_light", "point_light.vert", "point_light.frag");
    auto flashShader = createShader(app, "flash_light", "flash_light.vert", "flash_light.frag");
    auto screenShader = createShader(app, "screen", "screen.vert", "screen.frag");

    std::vector<FrameBufferTextureCreateInfo> frameBufferFormats = {
        TextureFormat::R8G8B8A8,
        TextureFormat::R16FG16F, // NORMAL XY
        TextureFormat::R16FG16F  // NORMAL Z / SPECULAR
    };

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(app, "frame_buffer", SamplesPerTexel::COUNT_1,
                                                             frameBufferFormats, FrameBufferDepthCreateInfo());
    app->getAssets().store(fpFrameBuffer, AssetStorageMode::PERMANENT);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("frame_buffer", fpFrameBuffer));

    auto outputs = fpFrameBuffer->getOutputs();
    std::vector<std::shared_ptr<SampledTexture>> textures;
    textures.reserve(outputs.size());

    for (auto& output : outputs) {
        textures.push_back(SampledTexture::create(app, output.resolvedTexture));
    }

    std::vector<FrameBufferTextureCreateInfo> screenFormats = {TextureFormat::R8G8B8A8};
    auto screenFrameBuffer =
        std::make_shared<SimpleFrameBuffer>(app, "screen", SamplesPerTexel::COUNT_1, screenFormats);
    app->getAssets().store(screenFrameBuffer, AssetStorageMode::PERMANENT);
    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("screen", screenFrameBuffer));

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

std::shared_ptr<SampledTexture> loadSkybox(Room* room)
{
    static const std::vector<std::string> PATHS = {
        "resource/Skybox/right.jpg",  "resource/Skybox/left.jpg",  "resource/Skybox/top.jpg",
        "resource/Skybox/bottom.jpg", "resource/Skybox/front.jpg", "resource/Skybox/back.jpg",
    };

    TextureCreateInfo info;
    info.layers = 6;
    info.mipmaps = 10;
    info.viewType = TextureViewType::CUBE;

    auto texture = Texture::createTextureFromFiles(room->getApplication(), "skybox", PATHS, info);
    auto view = TextureView::create(room->getApplication(), "skybox", TextureViewCreateInfo(), texture);
    return SampledTexture::create(room->getApplication(), "skybox", std::make_shared<MutableAsset<TextureView>>(view));
}

std::shared_ptr<Room> getTestRoom(Application* application)
{
    auto room = std::make_shared<Room>(application);
    auto fpFrameBuffer = initRender(room.get());
    auto screenFrameBuffer = application->getAssets().get<FrameBuffer>("screen").value();

    auto skybox = loadSkybox(room.get());
    room->getApplication()->getRender()->getGlobalUniformBuffer().setTexture(1, skybox);

    auto cameraController = room->newGameObject();
    cameraController->newComponent<GlobalParametersUpdaterComponent>();
    auto cameraMovement = cameraController->newComponent<CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto imgui = room->newGameObject();
    imgui->newComponent<LockMouseComponent>(cameraMovement);
    imgui->newComponent<DockSpaceComponent>();
    imgui->newComponent<ViewportComponent>(std::dynamic_pointer_cast<SimpleFrameBuffer>(screenFrameBuffer));
    auto goExplorer = imgui->newComponent<GameObjectExplorerComponent>();
    imgui->newComponent<SceneTreeComponent>(goExplorer);
    imgui->newComponent<DebugOverlayComponent>(false, 100);

    std::shared_ptr materialDescriptor = ShaderUniformDescriptor::ofImages(application, "descriptor", 1);

    auto shader = createShader(application, "deferred", "deferred.vert", "deferred.frag");

    MaterialCreateInfo clothMaterialInfo(fpFrameBuffer, shader);
    clothMaterialInfo.descriptions.uniform = materialDescriptor;
    clothMaterialInfo.descriptions.instance.push_back(DefaultInstancingData::getInstancingDescription());
    clothMaterialInfo.descriptions.vertex.push_back(TestVertex::getDescription());
    clothMaterialInfo.rasterizer.polygonMode = neon::PolygonMode::FILL;
    clothMaterialInfo.rasterizer.cullMode = neon::CullMode::NONE;

    auto material = std::make_shared<Material>(application, "cloth", clothMaterialInfo);

    auto physicManagerGO = room->newGameObject();
    auto physicsManager = physicManagerGO->newComponent<PhysicsManager>(IntegrationMode::IMPLICIT);

    auto cloth = room->newGameObject();
    cloth->setName("Cloth");
    cloth->newComponent<Cloth>(material, physicsManager, 20, 20);

    room->getCamera().lookAt({0, 1.0f, -1.0f});
    room->getCamera().setPosition({0.0f, 3.0f, 3.0f});

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
