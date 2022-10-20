#include <iostream>
#include <vector>

#include "engine/Engine.h"
#include "test/TestComponent.h"

constexpr int32_t WIDTH = 800;
constexpr int32_t HEIGHT = 600;

std::shared_ptr<Room> getTestRoom() {
    auto room = std::make_shared<Room>();
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
