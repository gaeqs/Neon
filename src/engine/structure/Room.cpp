//
// Created by grial on 19/10/22.
//

#include "Room.h"

#include <glm/glm.hpp>
#include <utility>
#include <unordered_set>

#include "GameObject.h"
#include <engine/Application.h>
#include <engine/render/GraphicComponent.h>

namespace neon {

    constexpr float DEFAULT_FRUSTUM_NEAR = 0.1f;
    constexpr float DEFAULT_FRUSTUM_FAR = 500.0f;
    constexpr float DEFAULT_FRUSTUM_FOV = glm::radians(100.0f); // RADIANS

    Room::Room(Application* application,
               const std::shared_ptr<ShaderUniformDescriptor>& descriptor) :
            _application(application),
            _camera(Frustum(DEFAULT_FRUSTUM_NEAR, DEFAULT_FRUSTUM_FAR, 1.0f,
                            DEFAULT_FRUSTUM_FOV)),
            _gameObjects(),
            _components(),
            _globalUniformDescriptor(descriptor),
            _globalUniformBuffer("global", descriptor),
            _usedModels(),
            _render(application, "render") {
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

    IdentifiableWrapper<GameObject> Room::newGameObject() {
        return _gameObjects.emplace(this);
    }

    void Room::destroyGameObject(IdentifiableWrapper<GameObject> gameObject) {
        _gameObjects.remove(gameObject.raw());
    }

    size_t Room::getGameObjectAmount() {
        return _gameObjects.size();
    }

    void Room::forEachGameObject(std::function<void(GameObject*)> consumer) {
        _gameObjects.forEach(std::move(consumer));
    }

    void Room::forEachGameObject(
            std::function<void(const GameObject*)> consumer) const {
        _gameObjects.forEach(std::move(consumer));
    }

    void Room::onKey(const KeyboardEvent& event) {
        DEBUG_PROFILE(getApplication()->getProfiler(), onKey);
        _components.invokeKeyEvent(getApplication()->getProfiler(), event);
    }

    void Room::onCursorMove(const CursorMoveEvent& event) {
        DEBUG_PROFILE(getApplication()->getProfiler(), onCursorMove);
        _components.invokeCursorMoveEvent(getApplication()->getProfiler(),
                                          event);
    }

    void Room::update(float deltaTime) {
        auto& p = getApplication()->getProfiler();
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
        auto& p = getApplication()->getProfiler();
        DEBUG_PROFILE(p, preDraw);
        _components.preDrawComponents(p);

        {
            DEBUG_PROFILE(p, models);
            _components.flushModels();
        }

        {
            DEBUG_PROFILE(p, uniformBuffers);
            _globalUniformBuffer.prepareForFrame();

            std::unordered_set<Material*> materials;

            for (const auto& [model, amount]: _usedModels) {
                for (int i = 0; i < model->getMeshesAmount(); ++i) {
                    auto& material = model->getMesh(i)->getMaterial();
                    materials.insert(material.get());
                }
            }

            for (const auto& material: materials) {
                material->getUniformBuffer().prepareForFrame();
            }
        }

    }

    void Room::draw() {
        DEBUG_PROFILE(getApplication()->getProfiler(), draw);
        _render.render(this);
    }

    const Render& Room::getRender() const {
        return _render;
    }

    Render& Room::getRender() {
        return _render;
    }

    void Room::markUsingModel(neon::Model* model) {
        ++_usedModels[model];
    }

    void Room::unmarkUsingModel(neon::Model* model) {
        auto it = _usedModels.find(model);
        if (it == _usedModels.end()) return;
        if (it->second <= 1) {
            _usedModels.erase(model);
        } else {
            --it->second;
        }
    }
}