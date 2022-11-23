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

Room::Room(Application* application,
           const std::shared_ptr<ShaderUniformDescriptor>& descriptor) :
        _application(application),
        _camera(Frustum(DEFAULT_FRUSTUM_NEAR, DEFAULT_FRUSTUM_FAR, 1.0f,
                        DEFAULT_FRUSTUM_FOV)),
        _gameObjects(),
        _components(),
        _textures(this),
        _models(this),
        _shaders(this),
        _materials(this),
        _globalUniformDescriptor(descriptor),
        _globalUniformBuffer(descriptor),
        _renderer() {
    _globalUniformBuffer.setBindingPoint(0);
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

const std::shared_ptr<ShaderUniformDescriptor>&
Room::getGlobalUniformDescriptor() const {
    return _globalUniformDescriptor;
}

const ShaderUniformBuffer& Room::getGlobalUniformBuffer() const {
    return _globalUniformBuffer;
}

ShaderUniformBuffer& Room::getGlobalUniformBuffer() {
    return _globalUniformBuffer;
}

const IdentifiableCollection<ShaderProgram>& Room::getShaders() const {
    return _shaders;
}

IdentifiableCollection<ShaderProgram>& Room::getShaders() {
    return _shaders;
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
    _globalUniformBuffer.prepareForFrame();
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

const IdentifiableCollection<Material>& Room::getMaterials() const {
    return _materials;
}

IdentifiableCollection<Material>& Room::getMaterials() {
    return _materials;
}
