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

#include "Line.h"
#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"

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
                                                        ShaderUniformBinding::uniformBuffer(sizeof(Timestamp)),
                                                        ShaderUniformBinding::image()};

    // The description of the global uniforms.
    auto globalDescriptor = std::make_shared<ShaderUniformDescriptor>(app, "default", globalBindings);

    // The render of the application.
    // We should set the render to the application before
    // we do anything else, as some components depend on
    // the application's render.
    auto render = std::make_shared<Render>(app, "default", globalDescriptor);
    app->setRender(render);

    // The format of the first frame buffer.
    std::vector<FrameBufferTextureCreateInfo> frameBufferFormats = {
        TextureFormat::R8G8B8A8,
        TextureFormat::R16FG16F, // NORMAL XY
        TextureFormat::R16FG16F  // NORMAL Z / SPECULAR
    };

    // Here we create the first frame buffer.
    // Just after creation, the frame buffer should be added
    // to the render as a render pass.
    // We'll use the default strategy for the rendering.
    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(app, "frame_buffer", SamplesPerTexel::COUNT_1,
                                                             frameBufferFormats, FrameBufferDepthCreateInfo());
    app->getAssets().store(fpFrameBuffer, AssetStorageMode::PERMANENT);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("frame_buffer", fpFrameBuffer));


    // Here we create the second frame buffer.
    // Just like the first frame buffer, we define the output,
    // create the frame buffer and add a render pass.
    std::vector<FrameBufferTextureCreateInfo> screenFormats = {TextureFormat::R8G8B8A8};
    auto screenFrameBuffer =
        std::make_shared<SimpleFrameBuffer>(app, "screen", SamplesPerTexel::COUNT_1, screenFormats);
    app->getAssets().store(screenFrameBuffer, AssetStorageMode::PERMANENT);
    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>("screen", screenFrameBuffer));

    // Here we create a model that renders the screen on
    // the second render pass.
    // This model is a screen plane that uses the
    // output textures of the first render pass.

    auto screenShader = createShader(app, "screen", "screen.vert", "screen.frag");

    // We can also create a GraphicComponent that handles
    // an instance of the model.
    // This option is more direct.
    auto screenModel = deferred_utils::createScreenModel(app, ModelCreateInfo(), "screen model");

    auto outputs = fpFrameBuffer->getOutputs();
    std::vector<std::shared_ptr<SampledTexture>> textures;
    textures.reserve(outputs.size());

    for (auto& output : outputs) {
        textures.push_back(SampledTexture::create(app, output.resolvedTexture));
    }

    std::shared_ptr<Material> screenMaterial = Material::create(
        room->getApplication(), "Screen Model", screenFrameBuffer, screenShader,
        deferred_utils::DeferredVertex::getDescription(), InputDescription(0, InputRate::INSTANCE), {}, textures);
    screenModel->addMaterial(screenMaterial);

    auto screenModelGO = room->newGameObject();
    screenModelGO->setName("Screen Model");
    screenModelGO->newComponent<GraphicComponent>(screenModel);

    // Finally, we create the swap chain buffer.
    // We split the screen render in two frame buffers
    // because of ImGUI.
    // ImGUI will use the result of screenFrameBuffer
    // to render a viewport.
    // Then, it will render its UI.
    // If you don't use ImGUI, you don't have to
    // split the screen render in two frame buffers.
    auto swapFrameBuffer = std::make_shared<SwapChainFrameBuffer>(app, "swap_chain", SamplesPerTexel::COUNT_1, false);
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
    application->getRender()->getGlobalUniformBuffer().setTexture(2, skybox);

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

    std::shared_ptr<ShaderUniformDescriptor> materialDescriptor =
        ShaderUniformDescriptor::ofImages(application, "descriptor", 1);

    auto shader = createShader(application, "deferred", "deferred.vert", "deferred.frag");

    MaterialCreateInfo lineMaterialInfo(fpFrameBuffer, shader);
    lineMaterialInfo.descriptions.uniform = materialDescriptor;
    lineMaterialInfo.descriptions.instance.push_back(DefaultInstancingData::getInstancingDescription());
    lineMaterialInfo.descriptions.vertex.push_back(TestVertex::getDescription());
    lineMaterialInfo.rasterizer.polygonMode = neon::PolygonMode::FILL;
    lineMaterialInfo.rasterizer.cullMode = neon::CullMode::NONE;
    lineMaterialInfo.rasterizer.lineWidth = 5.0f;

    lineMaterialInfo.topology = neon::PrimitiveTopology::LINE_STRIP;

    auto material = std::make_shared<Material>(application, "line", lineMaterialInfo);

    rush::BezierSegment<4, 3, float> seg1{
        rush::Vec3f{ 1.0f,   3.0f, 5.0f},
        rush::Vec3f{ 0.0f, -10.0f, 3.0f},
        rush::Vec3f{-3.0f,   0.0f, 2.0f},
        rush::Vec3f{20.0f,   3.0f, 5.0f}
    };

    auto seg2 = rush::BezierSegment<4, 3, float>::continuousTo(seg1, rush::Vec3f{10.0f, 6.0f, 0.0f},
                                                               rush::Vec3f{0.0f, 0.0f, 0.0f});

    rush::BezierCurve<2, 4, 3, float> curve{seg1, seg2};

    auto rectified = curve.rectified();

    auto line = room->newGameObject();
    line->setName("Line");
    line->newComponent<Box<decltype(rectified)>>(material, rectified, 500);

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
        f.basicFeatures.wideLines = true;
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
