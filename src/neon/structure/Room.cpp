//
// Created by grial on 19/10/22.
//

#include "Room.h"

#include <utility>
#include <unordered_set>
#include <neon/filesystem/FileSystem.h>

#include <neon/structure/GameObject.h>
#include <neon/structure/Component.h>
#include <neon/structure/Application.h>
#include <neon/render/GraphicComponent.h>

namespace neon {
    constexpr float DEFAULT_FRUSTUM_NEAR = 0.1f;
    constexpr float DEFAULT_FRUSTUM_FAR = 500.0f;
    constexpr float DEFAULT_FRUSTUM_FOV = 1.7453f; // RADIANS

    Room::Room(Application* application) : _application(application),
                                           _camera(Frustum(
                                               DEFAULT_FRUSTUM_NEAR,
                                               DEFAULT_FRUSTUM_FAR, 1.0f,
                                               DEFAULT_FRUSTUM_FOV)),
                                           _gameObjects(),
                                           _components(),

                                           _usedModels() {}

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

    IdentifiableWrapper<GameObject> Room::newGameObject() {
        return _gameObjects.emplace(this);
    }

    void Room::destroyGameObject(IdentifiableWrapper<GameObject> gameObject) {
        _gameObjects.remove(gameObject.raw());
    }

    void Room::destroyComponentLater(
        IdentifiableWrapper<neon::Component> component) {
        _destroyLater.insert(component);
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

    size_t Room::loadGameObjects(nlohmann::json json, AssetLoaderContext context) {
        std::vector<nlohmann::json> entries;
        if (json.is_array()) {
            entries = json;
        } else if (json.is_object()) {
            entries.push_back(json);
        }

        size_t count = 0;

        for (auto& jsonEntry: entries) {
            auto& name = jsonEntry["name"];
            if (!name.is_string()) continue;
            auto gameObject = newGameObject();
            gameObject->setName(name);
            ++count;

            if (auto& transform = jsonEntry["transform"]; transform.is_object()) {
                auto& position = transform["position"];
                auto& rotation = transform["rotation"];
                auto& scale = transform["scale"];
                if (position.is_object()) {
                    rush::Vec3f result = gameObject->getTransform().getPosition();
                    if (auto& x = position["x"]; x.is_number()) result[0] = x.get<float>();
                    if (auto& y = position["y"]; y.is_number()) result[1] = y.get<float>();
                    if (auto& z = position["z"]; z.is_number()) result[2] = z.get<float>();
                    gameObject->getTransform().setPosition(result);
                }
                if (rotation.is_object()) {
                    rush::Vec3f result = gameObject->getTransform().getRotation().euler();
                    if (auto& x = rotation["x"]; x.is_number()) result[0] = x.get<float>();
                    if (auto& y = rotation["y"]; y.is_number()) result[1] = y.get<float>();
                    if (auto& z = rotation["z"]; z.is_number()) result[2] = z.get<float>();
                    gameObject->getTransform().setRotation(rush::Quatf::euler(result));
                }
                if (scale.is_object()) {
                    rush::Vec3f result = gameObject->getTransform().getScale();
                    if (auto& x = scale["x"]; x.is_number()) result[0] = x.get<float>();
                    if (auto& y = scale["y"]; y.is_number()) result[1] = y.get<float>();
                    if (auto& z = scale["z"]; z.is_number()) result[2] = z.get<float>();
                    gameObject->getTransform().setScale(result);
                }
            }

            auto& jsonComponents = jsonEntry["components"];
            std::vector<nlohmann::json> components;
            if (jsonComponents.is_array()) {
                components = jsonComponents;
            } else if (jsonComponents.is_object()) {
                components.push_back(jsonComponents);
            }

            for (auto& jsonComponent: components) {
                auto& jsonType = jsonComponent["type"];
                if (!jsonType.is_string()) continue;
                auto type = ComponentRegister::instance()
                        .getEntry(jsonType.get<std::string>());
                if (!type.has_value() || !type.value().jsonCreator.has_value()) continue;
                type->jsonCreator.value()(*gameObject.raw(), jsonComponent, context);
            }
        }

        return count;
    }


    size_t Room::loadGameObjects(const std::filesystem::path& path, AssetLoaderContext context) {
        if (context.fileSystem == nullptr) return 0;

        context.path = context.path.has_value() ? context.path.value().parent_path() / path : path;
        auto file = context.fileSystem->readFile(context.path.value());
        if (!file.has_value()) return 0;
        auto json = file.value().toJson();
        if (!json.has_value()) return 0;

        return loadGameObjects(json.value(), context);
    }

    void Room::onKey(const KeyboardEvent& event) {
        DEBUG_PROFILE(getApplication()->getProfiler(), onKey);
        _components.invokeKeyEvent(getApplication()->getProfiler(), event);
    }

    void Room::onMouseButton(const MouseButtonEvent& event) {
        DEBUG_PROFILE(getApplication()->getProfiler(), onMouseButton);
        _components.invokeMouseButtonEvent(getApplication()->getProfiler(),
                                           event);
    }

    void Room::onCursorMove(const CursorMoveEvent& event) {
        DEBUG_PROFILE(getApplication()->getProfiler(), onCursorMove);
        _components.invokeCursorMoveEvent(getApplication()->getProfiler(),
                                          event);
    }

    void Room::onScroll(const neon::ScrollEvent& event) {
        DEBUG_PROFILE(getApplication()->getProfiler(), onCursorMove);
        _components.invokeScrollEvent(getApplication()->getProfiler(),
                                      event);
    }

    void Room::update(float deltaTime) {
        for (const auto& component: _destroyLater) {
            if (component.isValid()) {
                component->destroy();
            }
        }
        _destroyLater.clear();

        auto& p = getApplication()->getProfiler(); {
            DEBUG_PROFILE(p, update);
            _components.updateComponents(p, deltaTime);
        } {
            DEBUG_PROFILE(p, lateUpdate);
            _components.lateUpdateComponents(p, deltaTime);
        }
    }

    void Room::preDraw() {
        auto& p = getApplication()->getProfiler();
        auto* cb = _application->getCurrentCommandBuffer();

        DEBUG_PROFILE(p, preDraw);
        _components.preDrawComponents(p); {
            DEBUG_PROFILE(p, models);
            for (const auto& [model, amount]: _usedModels) {
                if (model->shouldAutoFlush()) {
                    for (auto& instanceData: model->getInstanceDatas()) {
                        instanceData->flush();
                    }
                }
                if (model->getUniformBuffer() != nullptr) {
                    model->getUniformBuffer()->prepareForFrame(cb);
                }
            }
        } {
            DEBUG_PROFILE(p, uniformBuffers);
            _application->getRender()->getGlobalUniformBuffer()
                    .prepareForFrame(cb);

            std::unordered_set<Material*> materials;

            for (const auto& [model, amount]: _usedModels) {
                for (int i = 0; i < model->getMeshesAmount(); ++i) {
                    for (const auto& mat: model->getDrawable(i)->getMaterials()) {
                        materials.insert(mat.get());
                    }
                }

                for (auto [location, buffer]: model->getUniformBufferBindings() | std::views::values) {
                    if (location == ModelBufferLocation::EXTRA && buffer != nullptr) {
                        buffer->prepareForFrame(cb);
                    }
                }
            }

            for (const auto& material: materials) {
                if (material->getUniformBuffer() != nullptr) {
                    material->getUniformBuffer()->prepareForFrame(cb);
                }
            }
        }
    }

    const std::unordered_map<Model*, uint32_t>& Room::usedModels() const {
        return _usedModels;
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
