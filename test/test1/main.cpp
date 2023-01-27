#include <iostream>
#include <vector>
#include <filesystem>

#include <engine/Engine.h>
#include <util/component/CameraMovementComponent.h>
#include <util/component/DebugOverlayComponent.h>
#include <util/component/DockSpaceComponent.h>
#include <util/component/ViewportComponent.h>
#include <util/component/SceneTreeComponent.h>
#include <util/component/GameObjectExplorerComponent.h>
#include <util/DeferredUtils.h>
#include <assimp/ModelLoader.h>

#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "ConstantRotationComponent.h"

constexpr int32_t WIDTH = 800;
constexpr int32_t HEIGHT = 600;

CMRC_DECLARE(shaders);

IdentifiableWrapper<ShaderProgram> createShader(Room* room,
                                                const std::string& vert,
                                                const std::string& frag) {
    auto defaultVert = cmrc::shaders::get_filesystem().open(vert);
    auto defaultFrag = cmrc::shaders::get_filesystem().open(frag);

    auto shader = room->getShaders().create();
    shader->addShader(ShaderType::VERTEX, defaultVert);
    shader->addShader(ShaderType::FRAGMENT, defaultFrag);

    auto result = shader->compile();
    if (result.has_value()) {
        std::cerr << result.value() << std::endl;
        throw std::runtime_error(result.value());
    }

    return shader;
}

std::shared_ptr<FrameBuffer> initRender(Room* room) {
    auto directionalShader = createShader(room, "directional_light.vert",
                                          "directional_light.frag");
    auto pointShader = createShader(room, "point_light.vert",
                                    "point_light.frag");
    auto flashShader = createShader(room, "flash_light.vert",
                                    "flash_light.frag");

    std::vector<TextureFormat> frameBufferFormats = {
            TextureFormat::R8G8B8A8,
            TextureFormat::R16FG16F, // NORMAL XY
            TextureFormat::R16FG16F // NORMAL Z / SPECULAR
    };

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(
            room, frameBufferFormats, true);

    room->getRender().addRenderPass(
            {fpFrameBuffer, RenderPassStrategy::defaultStrategy});

    auto outputColor = deferred_utils::createLightSystem(
            room,
            fpFrameBuffer->getTextures(),
            TextureFormat::R8G8B8A8,
            directionalShader,
            pointShader,
            flashShader
    );

    auto scFrameBuffer = std::make_shared<SwapChainFrameBuffer>(
            room, false);

    room->getRender().addRenderPass(
            {scFrameBuffer, RenderPassStrategy::defaultStrategy});

    return fpFrameBuffer;
}

void loadSansModels(Application* application, Room* room,
                    const std::shared_ptr<FrameBuffer>& target) {

    std::vector<ShaderUniformBinding> sansMaterialBindings = {
            ShaderUniformBinding{UniformBindingType::IMAGE, 0},
            ShaderUniformBinding{UniformBindingType::IMAGE, 0}
    };


    auto sansMaterialDescriptor = std::make_shared<ShaderUniformDescriptor>(
            application,
            sansMaterialBindings
    );


    auto shader = createShader(room, "deferred.vert", "deferred.frag");

    auto sansResult = ModelLoader(room).loadModel
            <TestVertex, DefaultInstancingData>(
            target,
            shader,
            sansMaterialDescriptor,
            R"(resource/Sans)",
            "Sans.obj");

    if (!sansResult.valid) {
        std::cout << "Couldn't load Sans model!" << std::endl;
        std::cout << std::filesystem::current_path() << std::endl;
        exit(1);
    }
    auto sansModel = sansResult.model;

    constexpr int AMOUNT = 1024 * 1;
    int q = static_cast<int>(std::sqrt(AMOUNT));
    for (int i = 0; i < AMOUNT; i++) {
        auto sans = room->newGameObject();
        sans->newComponent<GraphicComponent>(sansModel);
        sans->newComponent<ConstantRotationComponent>();

        if(i == 0) {
            auto sans2 = room->newGameObject();
            sans2->newComponent<GraphicComponent>(sansModel);
            sans2->newComponent<ConstantRotationComponent>();
            sans2->setParent(sans);
            sans2->getTransform().setPosition(glm::vec3(-5.0f, 5.0f, 0.0f));
            sans2->setName("Children Sans");
        }

        float x = static_cast<float>(i % q) * 3.0f;
        float z = static_cast<float>(i / q) * 3.0f; // NOLINT(bugprone-integer-division)
        sans->getTransform().setPosition(glm::vec3(x, 0, z));
        sans->setName("Sans " + std::to_string(i));
    }
}

std::shared_ptr<Room> getTestRoom(Application* application) {

    std::vector<ShaderUniformBinding> globalBindings = {ShaderUniformBinding{
            UniformBindingType::BUFFER, sizeof(GlobalParameters)
    }};

    auto globalDescriptor = std::make_shared<ShaderUniformDescriptor>(
            application, globalBindings
    );

    auto room = std::make_shared<Room>(application, globalDescriptor);

    auto fpFrameBuffer = initRender(room.get());

    auto cameraController = room->newGameObject();
    auto cameraMovement = cameraController->newComponent<CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto parameterUpdater = room->newGameObject();
    parameterUpdater->newComponent<GlobalParametersUpdaterComponent>();
    parameterUpdater->newComponent<LockMouseComponent>(cameraMovement);
    parameterUpdater->newComponent<DockSpaceComponent>();
    parameterUpdater->newComponent<ViewportComponent>();
    auto goExplorer = parameterUpdater->newComponent<GameObjectExplorerComponent>();
    parameterUpdater->newComponent<SceneTreeComponent>(goExplorer);
    parameterUpdater->newComponent<DebugOverlayComponent>(false, 100);

    auto directionalLight = room->newGameObject();
    directionalLight->newComponent<DirectionalLight>();

    auto directionalLight2 = room->newGameObject();
    directionalLight2->newComponent<DirectionalLight>();
    directionalLight2->getTransform().lookAt(glm::vec3(1.0f, 0.0f, 0.0f));

    auto pointLightGO = room->newGameObject();
    auto pointLight = pointLightGO->newComponent<PointLight>();
    pointLightGO->getTransform().setPosition({5.0f, 7.0f, 5.0f});
    pointLight->setDiffuseColor({1.0f, 0.0f, 0.0f});
    pointLight->setConstantAttenuation(0.01f);
    pointLight->setLinearAttenuation(0.2);
    pointLight->setQuadraticAttenuation(0.1);

    auto flashLightGO = room->newGameObject();
    auto flashLight = flashLightGO->newComponent<FlashLight>();
    flashLightGO->getTransform().setPosition({10.0f, 7.0f, 10.0f});
    flashLightGO->getTransform().rotate(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
    flashLight->setDiffuseColor({0.0f, 1.0f, 0.0f});
    flashLight->setConstantAttenuation(0.01f);
    flashLight->setLinearAttenuation(0.2);
    flashLight->setQuadraticAttenuation(0.1);


    loadSansModels(application, room.get(), fpFrameBuffer);

    return room;
}

int main() {
    std::srand(std::time(nullptr));

    Application application(WIDTH, HEIGHT);

    auto initResult = application.init("Neon");
    if (!initResult.isOk()) {
        std::cerr << "[GLFW INIT]\t" << initResult.getError() << std::endl;
        return EXIT_FAILURE;
    }

    application.setRoom(getTestRoom(&application));

    auto loopResult = application.startGameLoop();
    if (loopResult.isOk()) {
        std::cout << "[APPLICATION]\tApplication closed. "
                  << loopResult.getResult() << " frames generated."
                  << std::endl;
    } else {
        std::cout << "[APPLICATION]\tUnexpected game loop error: "
                  << loopResult.getError()
                  << std::endl;
    }

    application.setRoom(nullptr);

    return EXIT_SUCCESS;
}
