#include <iostream>
#include <vector>
#include <filesystem>

#include <engine/Engine.h>
#include <util/component/CameraMovementComponent.h>
#include <util/component/DebugOverlayComponent.h>
#include <util/DeferredUtils.h>
#include <assimp/ModelLoader.h>

#include "TestComponent.h"
#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "ConstantRotationComponent.h"

constexpr int32_t WIDTH = 800;
constexpr int32_t HEIGHT = 600;

CMRC_DECLARE(shaders);

std::shared_ptr<FrameBuffer> initRender(Room* room) {
    std::vector<TextureFormat> frameBufferFormats = {
            TextureFormat::R8G8B8A8,
            TextureFormat::R16FG16F // NORMAL
    };

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(
            room, frameBufferFormats, true);


    room->getRender().addRenderPass(
            {fpFrameBuffer, RenderPassStrategy::defaultStrategy});


    auto extraTextures = fpFrameBuffer->getTextures();
    extraTextures = std::vector<IdentifiableWrapper<Texture>>(
            extraTextures.begin() + 1,
            extraTextures.end());
    auto outputColor = deferred_utils::createLightSystem(
            room,
            fpFrameBuffer->getTextures().front(),
            extraTextures,
            TextureFormat::R8G8B8A8,
            nullptr,
            nullptr,
            nullptr
    );

    auto scFrameBuffer = std::make_shared<SwapChainFrameBuffer>(
            room, false);

    room->getRender().addRenderPass(
            {scFrameBuffer, RenderPassStrategy::defaultStrategy});

    auto defaultVert = cmrc::shaders::get_filesystem().open("screen.vert");
    auto defaultFrag = cmrc::shaders::get_filesystem().open("screen.frag");

    auto shader = room->getShaders().create();
    shader->addShader(ShaderType::VERTEX, defaultVert);
    shader->addShader(ShaderType::FRAGMENT, defaultFrag);

    auto result = shader->compile();
    if (result.has_value()) {
        std::cerr << result.value() << std::endl;
        throw std::runtime_error(result.value());
    }

    auto textures = fpFrameBuffer->getTextures();
    textures[0] = outputColor;

    deferred_utils::createScreenModel(
            room,
            textures,
            scFrameBuffer,
            shader
    );

    return fpFrameBuffer;
}

void loadSansModels(Application* application, Room* room,
                    const std::shared_ptr<FrameBuffer>& target) {

    std::vector<ShaderUniformBinding> sansMaterialBindings = {
            ShaderUniformBinding{
                    UniformBindingType::IMAGE, 0
            }};


    auto sansMaterialDescriptor = std::make_shared<ShaderUniformDescriptor>(
            application,
            sansMaterialBindings
    );


    auto defaultVert = cmrc::shaders::get_filesystem().open("deferred.vert");
    auto defaultFrag = cmrc::shaders::get_filesystem().open("deferred.frag");

    auto shader = room->getShaders().create();
    shader->addShader(ShaderType::VERTEX, defaultVert);
    shader->addShader(ShaderType::FRAGMENT, defaultFrag);

    auto result = shader->compile();
    if (result.has_value()) {
        std::cerr << result.value() << std::endl;
        throw std::runtime_error(result.value());
    }

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

        float x = static_cast<float>(i % q) * 3.0f;
        float z = static_cast<float>(i / q) * 3.0f;
        sans->getTransform().setPosition(glm::vec3(x, 0, z));
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

    auto gameObject = room->newGameObject();
    gameObject->newComponent<TestComponent>();
    gameObject->getTransform().setPosition(glm::vec3(0.0f, 0.0f, -1.0f));

    auto gameObject2 = room->newGameObject();
    gameObject2->newComponent<TestComponent>();

    gameObject2->setParent(gameObject);
    gameObject2->getTransform().setPosition(glm::vec3(0.0f, -1.0f, 0.0f));

    auto cameraController = room->newGameObject();
    auto cameraMovement = cameraController->newComponent<CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto parameterUpdater = room->newGameObject();
    parameterUpdater->newComponent<GlobalParametersUpdaterComponent>();
    parameterUpdater->newComponent<LockMouseComponent>(cameraMovement);
    parameterUpdater->newComponent<DebugOverlayComponent>(100);

    loadSansModels(application, room.get(), fpFrameBuffer);

    return room;
}

int main() {

    Application application(WIDTH, HEIGHT);

    auto initResult = application.init("Test");
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
