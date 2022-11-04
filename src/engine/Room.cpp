//
// Created by grial on 19/10/22.
//

#include "Room.h"

#include <glm/glm.hpp>

#include <engine/GameObject.h>
#include <engine/Application.h>
#include <engine/GraphicComponent.h>

constexpr float DEFAULT_FRUSTUM_NEAR = 0.1f;
constexpr float DEFAULT_FRUSTUM_FAR = 1000.0f;
constexpr float DEFAULT_FRUSTUM_FOV = glm::radians(100.0f); // RADIANS

Room::Room() :
        _application(nullptr),
        _camera(Frustum(DEFAULT_FRUSTUM_NEAR, DEFAULT_FRUSTUM_FAR, 1.0f,
                        DEFAULT_FRUSTUM_FOV)),
        _gameObjects(),
        _components(this),
        _textures(),
        _models(),
        _renderer() {
}

Room::~Room() {
    for (auto& item: _gameObjects) {
        item.destroy();
    }
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

const IdentifiableCollection<Model>& Room::getModels() const {
    return _models;
}

IdentifiableCollection<Model>& Room::getModels() {
    return _models;
}

const IdentifiableCollection<ShaderUniformBuffer>&
Room::getShaderUniformBuffers() const {
    return _shaderUniformBuffers;
}

IdentifiableCollection<ShaderUniformBuffer>& Room::getShaderUniformBuffers() {
    return _shaderUniformBuffers;
}

IdentifiableWrapper<GameObject> Room::newGameObject() {
    return _gameObjects.emplace(this);
}

void Room::destroyGameObject(IdentifiableWrapper<GameObject> gameObject) {
    _gameObjects.remove(gameObject.raw());
}

void Room::onResize() {
    if (_application->getWidth() > 0 && _application->getHeight() > 0) {
        _camera.setFrustum(_camera.getFrustum().withAspectRatio(
                _application->getAspectRatio()));
    }
}

void Room::onKey(const KeyboardEvent& event) {
    _components.invokeKeyEvent(event);
}

void Room::onCursorMove(const CursorMoveEvent& event) {
    _components.invokeCursorMoveEvent(event);
}

void Room::update(float deltaTime) {
    _components.updateComponents(deltaTime);
}

void Room::draw() {
    _components.drawGraphicComponents(this);
}

const std::shared_ptr<Renderer>& Room::getRenderer() const {
    return _renderer;
}

std::shared_ptr<Renderer>& Room::getRenderer() {
    return _renderer;
}

void Room::setRenderer(const std::shared_ptr<Renderer>& renderer) {
    _renderer = renderer;
}
