//
// Created by grial on 19/10/22.
//

#include <iostream>
#include "Room.h"
#include "Application.h"
#include "GraphicComponent.h"
#include "GameObject.h"

constexpr float_t DEFAULT_FRUSTUM_NEAR = 0.1f;
constexpr float_t DEFAULT_FRUSTUM_FAR = 1000.0f;
constexpr float_t DEFAULT_FRUSTUM_FOV = 1000.0f;

Room::Room() :
        _camera(Frustum(DEFAULT_FRUSTUM_NEAR, DEFAULT_FRUSTUM_FAR, 1.0f, DEFAULT_FRUSTUM_FOV)),
        _components(),
        _application(nullptr) {
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

void Room::bindApplication(Application* application) {
    _application = application;
}

GameObject& Room::newGameObject() {
    return _gameObjects[0];//_gameObjects.emplace_back(this);
}

void Room::onResize() {
    _camera.setFrustum(_camera.getFrustum().withAspectRatio(_application->getAspectRatio()));
}

void Room::update() {

}

void Room::draw() {
    auto& graphicComponentsRaw = _components[typeid(GraphicComponent)];
    auto* graphicComponents = static_cast<std::vector<GraphicComponent>*>(graphicComponentsRaw.get());
    std::cout << graphicComponents->size() << std::endl;
}
