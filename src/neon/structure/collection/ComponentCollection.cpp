//
// Created by gaelr on 20/10/2022.
//

#include "ComponentCollection.h"

#include <memory>
#include <unordered_set>

#include <neon/structure/Room.h>
#include <neon/structure/Component.h>
#include <neon/render/GraphicComponent.h>
#include <neon/render/model/Model.h>

namespace neon
{
    ComponentCollection::ComponentCollection()
    {
    }

    void ComponentCollection::destroyComponent(const IdentifiableWrapper<Component>& component)
    {
        auto& type = typeid(*component.raw());
        auto it = _components.find(type);
        if (it == _components.end()) {
            return;
        }

        if (component && component->hasStarted() && component->isEnabled()) {
            component->onDisable();
        }

        auto collection = std::reinterpret_pointer_cast<AbstractClusteredLinkedCollection>(it->second.second);
        if (!collection->erase(component.raw())) {
            std::cerr << "Failed to erase component: " << typeid(Component).name() << std::endl;
        }
    }

    void ComponentCollection::invokeKeyEvent(Profiler& profiler, const KeyboardEvent& event)
    {
        flushNotStartedComponents();
        for (const auto& [type, data] : _components) {
            if (!data.first.onKey) {
                continue;
            }

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([&event](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled() && component->hasStarted()) {
                    component->onKey(event);
                }
            });
        }
    }

    void ComponentCollection::invokeCharEvent(Profiler& profiler, const CharEvent& event)
    {
        flushNotStartedComponents();
        for (const auto& [type, data] : _components) {
            if (!data.first.onChar) {
                continue;
            }

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([&event](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled() && component->hasStarted()) {
                    component->onChar(event);
                }
            });
        }
    }

    void ComponentCollection::invokeMouseButtonEvent(neon::Profiler& profiler, const MouseButtonEvent& event)
    {
        flushNotStartedComponents();
        for (const auto& [type, data] : _components) {
            if (!data.first.onMouseButton) {
                continue;
            }

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([&event](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled() && component->hasStarted()) {
                    component->onMouseButton(event);
                }
            });
        }
    }

    void ComponentCollection::invokeCursorMoveEvent(Profiler& profiler, const CursorMoveEvent& event)
    {
        flushNotStartedComponents();
        for (const auto& [type, data] : _components) {
            if (!data.first.onCursorMove) {
                continue;
            }

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([&event](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled() && component->hasStarted()) {
                    component->onCursorMove(event);
                }
            });
        }
    }

    void ComponentCollection::invokeScrollEvent(Profiler& profiler, const ScrollEvent& event)
    {
        flushNotStartedComponents();
        for (const auto& [type, data] : _components) {
            if (!data.first.onScroll) {
                continue;
            }

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([&event](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled() && component->hasStarted()) {
                    component->onScroll(event);
                }
            });
        }
    }

    void ComponentCollection::updateComponents(Profiler& profiler, float deltaTime)
    {
        flushNotStartedComponents();
        for (const auto& [type, data] : _components) {
            if (!data.first.onUpdate) {
                continue;
            }

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([deltaTime](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled() && component->hasStarted()) {
                    component->onUpdate(deltaTime);
                }
            });
        }
    }

    void ComponentCollection::lateUpdateComponents(Profiler& profiler, float deltaTime)
    {
        flushNotStartedComponents();
        for (const auto& [type, data] : _components) {
            if (!data.first.onLateUpdate) {
                continue;
            }

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([deltaTime](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled() && component->hasStarted()) {
                    component->onLateUpdate(deltaTime);
                }
            });
        }
    }

    void ComponentCollection::preDrawComponents(Profiler& profiler)
    {
        flushNotStartedComponents();
        for (const auto& [type, data] : _components) {
            if (!data.first.onPreDraw) {
                continue;
            }

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled() && component->hasStarted()) {
                    component->onPreDraw();
                }
            });
        }
    }

    void ComponentCollection::latePreDrawComponents(Profiler& profiler)
    {
        flushNotStartedComponents();
        for (const auto& [type, data] : _components) {
            if (!data.first.onLatePreDraw) {
                continue;
            }

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled() && component->hasStarted()) {
                    component->onLatePreDraw();
                }
            });
        }
    }

    void ComponentCollection::callOnConstruction(void* rawComponent)
    {
        reinterpret_cast<Component*>(rawComponent)->onConstruction();
    }

    void ComponentCollection::flushNotStartedComponents()
    {
        for (; !_notStartedComponents.empty(); _notStartedComponents.pop()) {
            auto ptr = _notStartedComponents.front();

            if (ptr.isValid()) {
                if (ptr->isEnabled()) {
                    ptr->onEnable();
                }
                ptr->onStart();
                ptr->_started = true;
            }
        }
    }
} // namespace neon
