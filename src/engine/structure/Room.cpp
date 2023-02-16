//
// Created by grial on 19/10/22.
//

#include "Room.h"

#include <glm/glm.hpp>
#include <utility>

#include "GameObject.h"
#include <engine/Application.h>
#include <engine/render/GraphicComponent.h>

namespace neon {

    constexpr float DEFAULT_FRUSTUM_NEAR = 0.1f;
    constexpr float DEFAULT_FRUSTUM_FAR = 500.0f;
    constexpr float DEFAULT_FRUSTUM_FOV = glm::radians(100.0f); // RADIANS

    Room::Room(Application *application,
               const std::shared_ptr<ShaderUniformDescriptor> &descriptor) :
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
            _render(application) {
        _globalUniformBuffer.setBindingPoint(0);
    }

    Room::~Room() {
        for (auto &item: _gameObjects) {
            item.destroy();
        }
    }

    Application *Room::getApplication() const {
        return _application;
    }

    const Camera &Room::getCamera() const {
        return _camera;
    }

    Camera &Room::getCamera() {
        return _camera;
    }

    const ComponentCollection &Room::getComponents() const {
        return _components;
    }

    ComponentCollection &Room::getComponents() {
        return _components;
    }

    const TextureCollection &Room::getTextures() const {
        return _textures;
    }

    TextureCollection &Room::getTextures() {
        return _textures;
    }

    const IdentifiableCollection<Model> &Room::getModels() const {
        return _models;
    }

    IdentifiableCollection<Model> &Room::getModels() {
        return _models;
    }

    const std::shared_ptr<ShaderUniformDescriptor> &
    Room::getGlobalUniformDescriptor() const {
        return _globalUniformDescriptor;
    }

    const ShaderUniformBuffer &Room::getGlobalUniformBuffer() const {
        return _globalUniformBuffer;
    }

    ShaderUniformBuffer &Room::getGlobalUniformBuffer() {
        return _globalUniformBuffer;
    }

    const IdentifiableCollection<ShaderProgram> &Room::getShaders() const {
        return _shaders;
    }

    IdentifiableCollection<ShaderProgram> &Room::getShaders() {
        return _shaders;
    }

    IdentifiableWrapper<GameObject> Room::newGameObject() {
        return _gameObjects.emplace(this);
    }

    void Room::destroyGameObject(IdentifiableWrapper<GameObject> gameObject) {
        _gameObjects.remove(gameObject.raw());
    }

    size_t Room::getGameObjectAmount() {
        return _gameObjects.size();
    }

    void Room::forEachGameObject(std::function<void(GameObject *)> consumer) {
        _gameObjects.forEach(std::move(consumer));
    }

    void Room::forEachGameObject(
            std::function<void(const GameObject *)> consumer) const {
        _gameObjects.forEach(std::move(consumer));
    }

    void Room::onKey(const KeyboardEvent &event) {
        DEBUG_PROFILE(getApplication()->getProfiler(), onKey);
        _components.invokeKeyEvent(getApplication()->getProfiler(), event);
    }

    void Room::onCursorMove(const CursorMoveEvent &event) {
        DEBUG_PROFILE(getApplication()->getProfiler(), onCursorMove);
        _components.invokeCursorMoveEvent(getApplication()->getProfiler(), event);
    }

    void Room::update(float deltaTime) {
        auto &p = getApplication()->getProfiler();
        {
            DEBUG_PROFILE(getApplication()->getProfiler(), update);
            _components.updateComponents(p, deltaTime);
        }
        {
            DEBUG_PROFILE(getApplication()->getProfiler(), lateUpdate);
            _components.lateUpdateComponents(p, deltaTime);
        }
    }

    void Room::preDraw() {
        auto &p = getApplication()->getProfiler();
        DEBUG_PROFILE(p, preDraw);
        _components.preDrawComponents(p);

        {
            DEBUG_PROFILE(p, models);
            _models.forEach([](Model *m) {
                m->flush();
            });
        }

        {
            DEBUG_PROFILE(p, uniformBUffers);
            _globalUniformBuffer.prepareForFrame();
            _materials.forEach([](Material *material) {
                material->getUniformBuffer().prepareForFrame();
            });
        }

    }

    void Room::draw() {
        DEBUG_PROFILE(getApplication()->getProfiler(), draw);
        _render.render(this);
    }

    const IdentifiableCollection<Material> &Room::getMaterials() const {
        return _materials;
    }

    IdentifiableCollection<Material> &Room::getMaterials() {
        return _materials;
    }

    const Render &Room::getRender() const {
        return _render;
    }

    Render &Room::getRender() {
        return _render;
    }
}