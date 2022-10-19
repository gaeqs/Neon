//
// Created by grial on 19/10/22.
//

#include "Room.h"
#include "Application.h"

constexpr float_t DEFAULT_FRUSTUM_NEAR = 0.1f;
constexpr float_t DEFAULT_FRUSTUM_FAR = 1000.0f;
constexpr float_t DEFAULT_FRUSTUM_FOV = 1000.0f;

Room::Room() : _camera(Frustum(DEFAULT_FRUSTUM_NEAR, DEFAULT_FRUSTUM_FAR, 1.0f, DEFAULT_FRUSTUM_FOV)) {
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

void Room::onResize() {
    _camera.setFrustum(_camera.getFrustum().withAspectRatio(_application->getAspectRatio()));
}

void Room::update() {

}

void Room::draw() {

}