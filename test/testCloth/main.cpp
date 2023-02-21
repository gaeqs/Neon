#include <iostream>
#include <memory>
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
#include <util/ModelUtils.h>
#include <assimp/AssimpLoader.h>

#include "Cloth.h"
#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "cloth/PhysicsManager.h"
#include "engine/shader/MaterialCreateInfo.h"
#include "engine/shader/ShaderUniformBinding.h"

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
    auto screenShader = createShader(room, "screen.vert",
                                     "screen.frag");

    std::vector<TextureFormat> frameBufferFormats = {
            TextureFormat::R8G8B8A8,
            TextureFormat::R16FG16F, // NORMAL XY
            TextureFormat::R16FG16F // NORMAL Z / SPECULAR
    };

    auto fpFrameBuffer = std::make_shared<SimpleFrameBuffer>(
            room, frameBufferFormats, true);

    room->getRender().addRenderPass(
            {fpFrameBuffer, RenderPassStrategy::defaultStrategy});

    std::vector<TextureFormat> screenFormats = {TextureFormat::R8G8B8A8};
    auto screenFrameBuffer = std::make_shared<SimpleFrameBuffer>(
            room, screenFormats, false);
    room->getRender().addRenderPass(
            {screenFrameBuffer, RenderPassStrategy::defaultStrategy});

    auto textures = fpFrameBuffer->getTextures();
    auto screenModel = deferred_utils::createScreenModel(
            room,
            textures,
            screenFrameBuffer,
            screenShader
    );

    auto instance = screenModel->createInstance();
    if (!instance.isOk()) {
        throw std::runtime_error(instance.getError());
    }

    auto swapFrameBuffer = std::make_shared<SwapChainFrameBuffer>(
            room, false);

    room->getRender().addRenderPass(
            {swapFrameBuffer, RenderPassStrategy::defaultStrategy});

    return fpFrameBuffer;
}

IdentifiableWrapper<Texture> loadSkybox(Room* room) {
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

    return room->getTextures().createTextureFromFiles(PATHS, info);
}

std::shared_ptr<Room> getTestRoom(Application* application) {

    std::vector<ShaderUniformBinding> globalBindings = {
            ShaderUniformBinding{UniformBindingType::BUFFER,
                                 sizeof(GlobalParameters)},
            ShaderUniformBinding{UniformBindingType::IMAGE, 0}
    };

    auto globalDescriptor = std::make_shared<ShaderUniformDescriptor>(
            application, globalBindings
    );

    auto room = std::make_shared<Room>(application, globalDescriptor);

    auto skybox = loadSkybox(room.get());
    room->getGlobalUniformBuffer().setTexture(1, skybox);

    auto fpFrameBuffer = initRender(room.get());

    auto cameraController = room->newGameObject();
    cameraController->newComponent<GlobalParametersUpdaterComponent>();
    auto cameraMovement = cameraController->newComponent<CameraMovementComponent>();
    cameraMovement->setSpeed(10.0f);

    auto imgui = room->newGameObject();
    imgui->newComponent<LockMouseComponent>(cameraMovement);
    imgui->newComponent<DockSpaceComponent>();
    imgui->newComponent<ViewportComponent>();
    auto goExplorer = imgui->newComponent<GameObjectExplorerComponent>();
    imgui->newComponent<SceneTreeComponent>(goExplorer);
    imgui->newComponent<DebugOverlayComponent>(false, 100);


    std::shared_ptr<ShaderUniformDescriptor> materialDescriptor =
            ShaderUniformDescriptor::ofImages(application, 1);

    auto shader = createShader(room.get(), "deferred.vert", "deferred.frag");

    MaterialCreateInfo clothMaterialInfo(fpFrameBuffer, shader);
    clothMaterialInfo.descriptions.uniform = materialDescriptor;
    clothMaterialInfo.descriptions.instance = DefaultInstancingData::getInstancingDescription();
    clothMaterialInfo.descriptions.vertex = TestVertex::getDescription();

    auto material = room->getMaterials().create(clothMaterialInfo);

    auto physicManagerGO = room->newGameObject();
    auto physicsManager = physicManagerGO->newComponent<PhysicsManager>(
            IntegrationMode::IMPLICIT);

    auto cloth = room->newGameObject();
    cloth->setName("Cloth");
    cloth->newComponent<Cloth>(material, physicsManager, 20, 20);

    room->getCamera().lookAt(glm::normalize(glm::vec3(0, 1.0f, -1.0f)));
    room->getCamera().setPosition(glm::vec3(0.0f, 3.0f, 3.0f));

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
