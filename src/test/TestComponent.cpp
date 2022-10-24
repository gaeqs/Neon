//
// Created by gaelr on 20/10/2022.
//

#include <iostream>
#include "TestComponent.h"


TestComponent::~TestComponent() {
    std::cout << "Test component " << getId() << " destroyed!" << std::endl;
}

void TestComponent::onConstruction() {
    std::cout << "Test component created!" << std::endl;
}

void TestComponent::onStart() {
    std::cout << "Test component started!" << std::endl;
}

void TestComponent::onUpdate() {
    std::cout << "This a test component!" << std::endl;
    auto o = getGameObject();

    auto euler = glm::eulerAngles(o->getRoom()->getCamera().rotate(
            glm::vec3(0.0f, 1.0f, 0.0f), 0.1f));
    std::cout << euler.x << ", " << euler.y << ", " << euler.z << std::endl;
}
