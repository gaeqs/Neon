//
// Created by gaelr on 20/10/2022.
//

#include <iostream>
#include "TestComponent.h"
#include "TestVertex.h"

#include <util/Resource.h>

RESOURCE(imageResource, src_resource_test_png);

TestComponent::~TestComponent() {
    std::cout << "Test component " << getId() << " destroyed!" << std::endl;
}

void TestComponent::onConstruction() {
    std::cout << "Test component created!" << std::endl;
}

void TestComponent::onStart() {
    auto image = getGameObject()->getRoom()->getTextures()
            .createTextureFromPNG(imageResource);

    std::cout << image->getId() << std::endl;

    std::cout << "Test component started!" << std::endl;
    auto gComponent = getGameObject()->newComponent<GraphicComponent>();

    std::vector<TestVertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
            {{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f,  0.8f,  0.0f}, {1.0f, 0.0f, 0.0f}},
            {{-0.5f, 0.5f,  0.0f}, {1.0f, 1.0f, 1.0f}},
    };

    std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    auto model = getGameObject()->getRoom()->getModels()
            .createModel(vertices, indices);

    getGameObject()->getTransform().setPosition(glm::vec3(0.3f, 0.0f, 0.5f));

    gComponent->getMaterial().setShader("default");
    gComponent->setModel(model);
}

void TestComponent::onUpdate() {
}
