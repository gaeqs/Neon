//
// Created by grial on 19/10/22.
//

#include "Room.h"
#include <engine/GameObject.h>
#include <engine/Application.h>
#include <engine/GraphicComponent.h>

constexpr float DEFAULT_FRUSTUM_NEAR = 0.1f;
constexpr float DEFAULT_FRUSTUM_FAR = 1000.0f;
constexpr float DEFAULT_FRUSTUM_FOV = 1000.0f;

Room::Room() :
        _application(nullptr),
        _camera(Frustum(DEFAULT_FRUSTUM_NEAR, DEFAULT_FRUSTUM_FAR, 1.0f,
                        DEFAULT_FRUSTUM_FOV)),
        _gameObjects(),
        _components(),
        _textures(),
        _models() {
}

Room::~Room() {
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

const ComponentCollection& Room::getComponents() const {
    return _components;
}

ComponentCollection& Room::getComponents() {
    return _components;
}

const TextureCollection& Room::getTextures() const {
    return _textures;
}

TextureCollection& Room::getTextures() {
    return _textures;
}

const ModelCollection& Room::getModels() const {
    return _models;
}

ModelCollection& Room::getModels() {
    return _models;
}

IdentifiableWrapper<GameObject> Room::newGameObject() {
    return _gameObjects.emplace(this);
}

void Room::destroyGameObject(IdentifiableWrapper<GameObject> gameObject) {
    _gameObjects.remove(gameObject.raw());
}

void Room::onResize() {
    _camera.setFrustum(_camera.getFrustum().withAspectRatio(
            _application->getAspectRatio()));
}

void Room::update() {

}

void Room::draw() {
}
