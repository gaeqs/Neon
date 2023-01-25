//
// Created by gaelr on 20/10/2022.
//

#include <iostream>
#include "TestComponent.h"
#include "TestVertex.h"

#include "cmrc/cmrc.hpp"

CMRC_DECLARE(shaders);
CMRC_DECLARE(resources);

TestComponent::~TestComponent() {
    std::cout << "Test component " << getId() << " destroyed!" << std::endl;
}

void TestComponent::onConstruction() {
    std::cout << "Test component created!" << std::endl;
}

void TestComponent::onStart() {
    auto file = cmrc::resources::get_filesystem().open("test.png");
    auto image = getGameObject()->getRoom()->getTextures()
            .createTextureFromPNG(file);

    std::cout << "Test component started!" << std::endl;
    _graphic = getGameObject()->newComponent<GraphicComponent>();

    std::vector<TestVertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{0.5f,  -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{0.5f,  0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{-0.5f, 0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
    };

    std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    std::vector<std::unique_ptr<Mesh>> meshes;

    //auto mesh = std::make_unique<Mesh>(getRoom()->getApplication());
    //mesh->getMaterial().setImage("diffuse", image);
    //mesh->uploadVertexData(vertices, indices);
    //meshes.push_back(std::move(mesh));

    auto model = getRoom()->getModels().create(meshes);
    _graphic->setModel(model);
}

void TestComponent::onUpdate(float deltaTime) {
    getGameObject()->getTransform().rotate(glm::vec3(0, 0, 1), deltaTime);
}
