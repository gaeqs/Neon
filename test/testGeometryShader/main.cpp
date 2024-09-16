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

#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "ConstantRotationComponent.h"
#include "neon/render/shader/Material.h"
#include "neon/render/shader/MaterialCreateInfo.h"
#include "neon/render/shader/ShaderUniformBinding.h"

constexpr float WIDTH = 800;
constexpr float HEIGHT = 600;

CMRC_DECLARE(shaders);

using namespace neon;

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
                                            const std::string& frag,
                                            const std::string& geom = "") {
    auto shader = std::make_shared<ShaderProgram>(application, std::move(name));

    auto defaultVert = cmrc::shaders::get_filesystem().open(vert);
    auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

    shader->addShader(
        ShaderType::VERTEX,
        std::string(defaultVert.begin(), defaultVert.end())
    );

    shader->addShader(
        ShaderType::FRAGMENT,
        std::string(defaultFrag.begin(), defaultFrag.end())
    );

    if (!geom.empty()) {
        auto defaultGeom = cmrc::shaders::get_filesystem().open(geom);

        shader->addShader(
            ShaderType::GEOMETRY,
            std::string(defaultGeom.begin(), defaultGeom.end())
        );
    }

    auto result = shader->compile();
    if (result.has_value()) {
        application->getLogger().error(result.value());
        throw std::runtime_error(result.value());
    }

    return shader;
}


std::shared_ptr<FrameBuffer> initRender(Room* room) {
    auto* app = room->getApplication();

    // Bindings for the global uniforms.
    // In this application, we have a buffer of global parameters
    // and a skybox.
    std::vector<ShaderUniformBinding> globalBindings = {
        {UniformBindingType::BUFFER, sizeof(Matrices)},
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

    std::vector<FrameBufferTextureCreateInfo> frameBufferFormats = {
        TextureFormat::R8G8B8A8,
        TextureFormat::R16FG16F, // NORMAL XY
        TextureFormat::R16FG16F // NORMAL Z / SPECULAR
    };

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(
        app, frameBufferFormats, true);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(
        fpFrameBuffer));

    auto albedo = deferred_utils::createLightSystem(
        room,
        render.get(),
        fpFrameBuffer->getTextures(),
        TextureFormat::R8G8B8A8,
        directionalShader,
        pointShader,
        flashShader
    );

    std::vector<FrameBufferTextureCreateInfo> screenFormats =
            {TextureFormat::R8G8B8A8};
    auto screenFrameBuffer = std::make_shared<SimpleFrameBuffer>(
        app, screenFormats, false);
    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(
        screenFrameBuffer));

    auto textures = fpFrameBuffer->getTextures();
    textures[0] = albedo;

    std::shared_ptr<Material> screenMaterial = Material::create(
        room->getApplication(), "Screen Model",
        screenFrameBuffer, screenShader,
        deferred_utils::DeferredVertex::getDescription(),
        InputDescription(0, InputRate::INSTANCE),
        {}, textures);

    auto screenModel = deferred_utils::createScreenModel(
        room->getApplication(),
        ModelCreateInfo(),
        "Screen Model"
    );

    screenModel->addMaterial(screenMaterial);

    auto screenModelGO = room->newGameObject();
    screenModelGO->setName("Screen Model");
    screenModelGO->newComponent<GraphicComponent>(screenModel);

    auto swapFrameBuffer = std::make_shared<SwapChainFrameBuffer>(
        room, false);

    render->addRenderPass(std::make_shared<DefaultRenderPassStrategy>(
        swapFrameBuffer));

    return fpFrameBuffer;
}

void loadModels(Application* application, Room* room,
                const std::shared_ptr<FrameBuffer>& target) {
    std::shared_ptr<ShaderUniformDescriptor> materialDescriptor =
            ShaderUniformDescriptor::ofImages(application, "default", 2);

    auto shader = createShader(application,
                               "deferred",
                               "deferred.vert",
                               "deferred.frag",
                               "deferred.geom");

    // CUBE
    std::vector<ShaderUniformBinding> cubeMaterialBindings;

    std::shared_ptr<ShaderUniformDescriptor> cubeMaterialDescriptor;
    materialDescriptor = std::make_shared<ShaderUniformDescriptor>(
        room->getApplication(),
        "pointMaterialDescriptor",
        cubeMaterialBindings
    );

    MaterialCreateInfo cubeMaterialInfo(target, shader);
    cubeMaterialInfo.descriptions.uniform = materialDescriptor;
    cubeMaterialInfo.descriptions.vertex.
            push_back(TestVertex::getDescription());
    cubeMaterialInfo.descriptions.instance.push_back(
        DefaultInstancingData::getInstancingDescription());
    cubeMaterialInfo.topology = PrimitiveTopology::POINT_LIST;

    auto material = std::make_shared<Material>(application, "pointMaterial",
                                               cubeMaterialInfo);

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

    return Texture::createTextureFromFiles(room->getApplication(),
                                           "skybox", PATHS, info);
}

std::shared_ptr<Room> getTestRoom(Application* application) {
    auto room = std::make_shared<Room>(application);

    auto fpFrameBuffer = initRender(room.get());

    auto skybox = loadSkybox(room.get());
    application->getRender()->getGlobalUniformBuffer().setTexture(1, skybox);


    auto cameraController = room->newGameObject();
    auto cameraMovement = cameraController->newComponent<
        CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto parameterUpdater = room->newGameObject();
    parameterUpdater->newComponent<GlobalParametersUpdaterComponent>();
    parameterUpdater->newComponent<LockMouseComponent>(cameraMovement);
    parameterUpdater->newComponent<DockSpaceComponent>();
    parameterUpdater->newComponent<ViewportComponent>();
    auto goExplorer = parameterUpdater->newComponent<
        GameObjectExplorerComponent>();
    parameterUpdater->newComponent<SceneTreeComponent>(goExplorer);
    parameterUpdater->newComponent<DebugOverlayComponent>(false, 100);
    parameterUpdater->newComponent<LogComponent>();

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
    flashLight->setLinearAttenuation(0.2);
    flashLight->setQuadraticAttenuation(0.1);


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
        application.getLogger().done(MessageBuilder()
            .print("Application closed. ")
            .print(loopResult.getResult(), TextEffect::foreground4bits(2))
            .print(" frames generated."));
    } else {
        application.getLogger().done(MessageBuilder()
            .print("Unexpected game loop error: ")
            .print(loopResult.getError(), TextEffect::foreground4bits(1)));
    }

    application.setRoom(nullptr);

    return EXIT_SUCCESS;
}
