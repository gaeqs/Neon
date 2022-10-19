#include <iostream>
#include "engine/Application.h"
#include "util/ClusteredLinkedCollection.h"
#include <vector>

constexpr int32_t WIDTH = 800;
constexpr int32_t HEIGHT = 600;

struct Test {
    int _i;

    Test(int i) : _i(i) {
        std::cout << "Create " << _i << std::endl;
    }

    Test(const Test &) = delete;

    ~Test() {
        std::cout << "Destroy " << _i << std::endl;
    }
};

int main() {
    ClusteredLinkedCollection<Test, 1024> collection;
    collection.emplace(20);

    std::cout << "Finished." << std::endl;
    std::cout << "Size: " << collection.size() << std::endl;

    //for (const auto& item: collection) {
//
    //}

    Application application(WIDTH, HEIGHT);

    auto initResult = application.init();
    if (!initResult.isOk()) {
        std::cerr << "[GLFW INIT]\t" << initResult.getError() << std::endl;
        return EXIT_FAILURE;
    }

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
