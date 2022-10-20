//
// Created by grial on 19/10/22.
//

#include <iostream>
#include "Room.h"
#include "GameObject.h"
#include "Application.h"
#include "GraphicComponent.h"

constexpr float_t DEFAULT_FRUSTUM_NEAR = 0.1f;
constexpr float_t DEFAULT_FRUSTUM_FAR = 1000.0f;
constexpr float_t DEFAULT_FRUSTUM_FOV = 1000.0f;

Room::Room() :
        _camera(Frustum(DEFAULT_FRUSTUM_NEAR, DEFAULT_FRUSTUM_FAR, 1.0f, DEFAULT_FRUSTUM_FOV)),
        _components(),
        _gameObjects(new ClusteredLinkedCollection<GameObject>),
        _application(nullptr) {
}

Room::~Room() {
    delete _gameObjects;
}

Application* Room::getApplication() const {
    return _application;
}

const Camera& Room::getCamera() const {
    return _camera;
}

Camera& Room::getCamera() {
    return _camera;
}

GameObject* Room::newGameObject() {
    return _gameObjects->emplace(this);
}

void Room::onResize() {
    _camera.setFrustum(_camera.getFrustum().withAspectRatio(_application->getAspectRatio()));
}

void Room::update() {
    for (const auto& item: _components) {
        auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(item.second);
        ptr->forEachRaw([](void* ptr) {
            reinterpret_cast<Component*>(ptr)->onUpdate();
        });
    }
}

void Room::draw() {
    auto& graphicComponentsRaw = _components[typeid(GraphicComponent)];
    auto graphicComponents = std::static_pointer_cast<ClusteredLinkedCollection<GraphicComponent>>(
            graphicComponentsRaw);
    std::cout << graphicComponents->size() << std::endl;
}
