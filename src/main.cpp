#include <iostream>
#include <vector>

#include "engine/Engine.h"
#include "test/TestComponent.h"
#include "gl/GLShaderRenderer.h"
#include "test/TestShaderController.h"
#include "test/TestVertex.h"

constexpr int32_t WIDTH = 800;
constexpr int32_t HEIGHT = 600;

SHADER_RESOURCE(defaultVert, default_vert)
SHADER_RESOURCE(defaultFrag, default_frag)

std::shared_ptr<Room> getTestRoom() {
    auto renderer = std::make_shared<GLShaderRenderer>();
    auto shader = Shader::newShader(defaultVert, defaultFrag);
    if (!shader.isOk()) throw std::runtime_error(shader.getError());
    auto shaderController = std::make_shared<TestShaderController>(
            shader.getResult());
    renderer->insertShader("default", shaderController);

    auto room = std::make_shared<Room>();
    room->setRenderer(renderer);

    auto gameObject = room->newGameObject();
    gameObject->newComponent<TestComponent>();

    return room;
}

int main() {

    Application application(WIDTH, HEIGHT);

    auto initResult = application.init();
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
