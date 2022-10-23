#include <iostream>
#include <vector>

#include "engine/Engine.h"
#include "test/TestComponent.h"

constexpr int32_t WIDTH = 800;
constexpr int32_t HEIGHT = 600;

std::shared_ptr<Room> getTestRoom() {
    auto room = std::make_shared<Room>();
    auto gameObject = room->newGameObject();
    auto component = gameObject->newComponent<TestComponent>();
    auto component2 = gameObject->newComponent<TestComponent>();
    std::cout << (component != nullptr) << std::endl;
    std::cout << component2.isValid() << std::endl;
    gameObject->destroyComponent(component);
    std::cout << component.isValid() << std::endl;
    std::cout << component2.isValid() << std::endl;

    auto component3 = gameObject->newComponent<TestComponent>();
    std::cout << component3.isValid() << std::endl;
    std::cout << component2.isValid() << std::endl;
    gameObject->destroyComponent(component2);
    std::cout << component3.isValid() << std::endl;
    std::cout << component2.isValid() << std::endl;


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
