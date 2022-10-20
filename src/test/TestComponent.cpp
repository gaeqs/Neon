//
// Created by gaelr on 20/10/2022.
//

#include <iostream>
#include "TestComponent.h"


void TestComponent::onUpdate() {
    std::cout << "This a test component!" << std::endl;
    GameObject* o = getGameObject();

    // TODO rotation not working
    auto euler = glm::eulerAngles(o->getRoom()->getCamera().rotate(glm::vec3(0.0f, 1.0f, 0.0f), 0.1f));
    std::cout << euler.x << ", " << euler.y << ", " << euler.z << std::endl;
}