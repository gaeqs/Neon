//
// Created by gaelr on 19/10/2022.
//

#include "Component.h"

#include <neon/structure/GameObject.h>
#include <neon/structure/Room.h>

namespace neon
{
    uint64_t COMPONENT_ID_GENERATOR = 1;

    Component::Component() :
        _id(COMPONENT_ID_GENERATOR++),
        _started(false),
        _enabled(true),
        _gameObject(nullptr)
    {
    }

    uint64_t Component::getId() const
    {
        return _id;
    }

    IdentifiableWrapper<GameObject> Component::getGameObject() const
    {
        return _gameObject;
    }

    bool Component::hasStarted() const
    {
        return _started;
    }

    bool Component::isEnabled() const
    {
        return _enabled;
    }

    void Component::setEnabled(bool enabled)
    {
        _enabled = enabled;
    }

    void Component::destroy()
    {
        if (_gameObject.isValid()) {
            _gameObject->destroyComponent(this);
        }
    }

    void Component::destroyLater()
    {
        getRoom()->destroyComponentLater(this);
    }

    void Component::onConstruction()
    {
    }

    void Component::onStart()
    {
    }

    void Component::onUpdate(float deltaTime)
    {
    }

    void Component::onLateUpdate(float deltaTime)
    {
    }

    void Component::onPreDraw()
    {
    }

    void Component::onKey(const KeyboardEvent& event)
    {
    }

    void Component::onMouseButton(const neon::MouseButtonEvent& event)
    {
    }

    void Component::onCursorMove(const CursorMoveEvent& event)
    {
    }

    void Component::onScroll(const ScrollEvent& event)
    {
    }

    void Component::drawEditor()
    {
    }

    Application* Component::getApplication() const
    {
        return getRoom()->getApplication();
    }

    AssetCollection& Component::getAssets() const
    {
        return getRoom()->getApplication()->getAssets();
    }

    TaskRunner& Component::getTaskRunner() const
    {
        return getRoom()->getApplication()->getTaskRunner();
    }

    Logger& Component::getLogger() const
    {
        return logger;
    }

    void Component::info(const std::string& message) const
    {
        getLogger().info(message);
    }

    void Component::done(const std::string& message) const
    {
        getLogger().done(message);
    }

    void Component::debug(const std::string& message) const
    {
        getLogger().debug(message);
    }

    auto Component::warning(const std::string& message) -> void
    {
        getLogger().warning(message);
    }

    auto Component::error(const std::string& message) -> void
    {
        getLogger().error(message);
    }
} // namespace neon
