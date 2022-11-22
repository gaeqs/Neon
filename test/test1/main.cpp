#include <iostream>
#include <vector>
#include <filesystem>

#include <engine/Engine.h>
#include <engine/shader/ShaderProgram.h>
#include <vulkan/VKShaderRenderer.h>
#include <util/component/CameraMovementComponent.h>
#include <assimp/ModelLoader.h>

#include "TestComponent.h"
#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"
#include "LockMouseComponent.h"
#include "ConstantRotationComponent.h"

constexpr int32_t WIDTH = 800;
constexpr int32_t HEIGHT = 600;

CMRC_DECLARE(shaders);

std::shared_ptr<Room> getTestRoom(Application* application) {

    std::vector<ShaderUniformBinding> globalBindings = {ShaderUniformBinding{
            UniformBindingType::BUFFER, sizeof(GlobalParameters)
    }};

    auto globalDescriptor = std::make_shared<ShaderUniformDescriptor>(
            application, globalBindings
    );

    auto room = std::make_shared<Room>(application, globalDescriptor);

    auto renderer = std::make_shared<VKShaderRenderer>(application);

    auto defaultVert = cmrc::shaders::get_filesystem().open("default.vert");
    auto defaultFrag = cmrc::shaders::get_filesystem().open("default.frag");

    auto shader = std::make_shared<ShaderProgram>(room.get());
    shader->addShader(ShaderType::VERTEX, defaultVert);
    shader->addShader(ShaderType::FRAGMENT, defaultFrag);

    auto result = shader->compile();
    if (result.has_value()) throw std::runtime_error(result.value());
    renderer->insertShader("default", shader);

    room->setRenderer(renderer);

    auto parametersUpdater = room->newGameObject();
    parametersUpdater->newComponent<GlobalParametersUpdaterComponent>();
    parametersUpdater->newComponent<LockMouseComponent>();

    auto gameObject = room->newGameObject();
    gameObject->newComponent<TestComponent>();
    gameObject->getTransform().setPosition(glm::vec3(0.0f, 0.0f, -1.0f));

    auto gameObject2 = room->newGameObject();
    gameObject2->newComponent<TestComponent>();

    gameObject2->setParent(gameObject);
    gameObject2->getTransform().setPosition(glm::vec3(0.0f, -1.0f, 0.0f));

    auto cameraController = room->newGameObject();
    cameraController->newComponent<CameraMovementComponent>();


    auto sansResult = ModelLoader(room).loadModel<TestVertex>(
            R"(resource/Sans)", "Sans.obj");

    if (!sansResult.valid) {
        std::cout << "Couldn't load Sans model!" << std::endl;
        std::cout << std::filesystem::current_path() << std::endl;
        exit(1);
    }
    auto sansModel = sansResult.model;
    sansModel->setShader("default");

    int q = static_cast<int>(std::sqrt(100));
    for (int i = 0; i < 100; i++) {
        auto sans = room->newGameObject();
        sans->newComponent<GraphicComponent>(sansModel);
        sans->newComponent<ConstantRotationComponent>();

        float x = static_cast<float>(i % q) * 3.0f;
        float y = static_cast<float>(i / q) * 3.0f;
        sans->getTransform().setPosition(glm::vec3(x, y, 0));
    }

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
    return EXIT_SUCCESS;
}
