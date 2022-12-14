#include <iostream>
#include <vector>

#include <engine/Engine.h>
#include <gl/GLShaderRenderer.h>
#include "util/component/CameraMovementComponent.h"
#include <assimp/ModelLoader.h>
#include "TestComponent.h"
#include "TestVertex.h"
#include "GlobalParametersUpdaterComponent.h"

constexpr int32_t WIDTH = 800;
constexpr int32_t HEIGHT = 600;

CMRC_DECLARE(shaders);

std::shared_ptr<Room> getTestRoom() {
    auto renderer = std::make_shared<GLShaderRenderer>();

    auto defaultVert = cmrc::shaders::get_filesystem().open("default.vert");
    auto defaultFrag = cmrc::shaders::get_filesystem().open("default.frag");

    auto shader = Shader::newShader(defaultVert, defaultFrag);
    if (!shader.isOk()) throw std::runtime_error(shader.getError());
    renderer->insertShader("default", shader.getResult());

    auto room = std::make_shared<Room>();
    room->setRenderer(renderer);

    auto parametersUpdater = room->newGameObject();
    parametersUpdater->newComponent<GlobalParametersUpdaterComponent>();

    auto gameObject = room->newGameObject();
    gameObject->newComponent<TestComponent>();
    gameObject->getTransform().setPosition(glm::vec3(0.0f, 0.0f, -1.0f));

    auto gameObject2 = room->newGameObject();
    gameObject2->newComponent<TestComponent>();

    gameObject2->setParent(gameObject);
    gameObject2->getTransform().setPosition(glm::vec3(0.0f, -1.0f, 0.0f));

    auto cameraController = room->newGameObject();
    cameraController->newComponent<CameraMovementComponent>();


    auto sansLoader = ModelLoader(room);
    auto sansModel = sansLoader.loadModel<TestVertex>(
            R"(resource/Sans)", "Sans.obj").model;
    sansModel->setShader("default");

    for (int i = 0; i < 100; i++) {
        auto sans = room->newGameObject();
        sans->newComponent<GraphicComponent>(sansModel);
        sans->getTransform().setPosition(glm::vec3(i, 0, 0));
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

    application.setRoom(getTestRoom());

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
