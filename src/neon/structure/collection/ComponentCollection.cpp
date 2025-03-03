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

namespace neon {
    ComponentCollection::ComponentCollection() :
        _components(),
        _notStartedComponents() {}

    void ComponentCollection::destroyComponent(
        const IdentifiableWrapper<Component>& component) {
        auto& type = typeid(*component.raw());
        auto it = _components.find(type);
        if (it == _components.end()) return;

        auto collection = std::reinterpret_pointer_cast<
            AbstractClusteredLinkedCollection>(it->second.second);
        if (!collection->erase(component.raw())) {
            std::cerr << "Failed to erase component: " << typeid(Component).name() << std::endl;
        }
    }

    void ComponentCollection::invokeKeyEvent(Profiler& profiler,
                                             const KeyboardEvent& event) {
        flushNotStartedComponents();
        for (const auto& [type, data]: _components) {
            if (!data.first.onKey) continue;

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast
                    <AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([&event](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled()) {
                    component->onKey(event);
                }
            });
        }
    }

    void ComponentCollection::invokeMouseButtonEvent(neon::Profiler& profiler,
                                                     const MouseButtonEvent& event) {
        flushNotStartedComponents();
        for (const auto& [type, data]: _components) {
            if (!data.first.onMouseButton) continue;

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast
                    <AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([&event](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled()) {
                    component->onMouseButton(event);
                }
            });
        }
    }

    void ComponentCollection::invokeCursorMoveEvent(Profiler& profiler,
                                                    const CursorMoveEvent& event) {
        flushNotStartedComponents();
        for (const auto& [type, data]: _components) {
            if (!data.first.onCursorMove) continue;

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast
                    <AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([&event](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled()) {
                    component->onCursorMove(event);
                }
            });
        }
    }

    void ComponentCollection::invokeScrollEvent(Profiler& profiler,
                                                const ScrollEvent& event) {
        flushNotStartedComponents();
        for (const auto& [type, data]: _components) {
            if (!data.first.onScroll) continue;

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast
                    <AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([&event](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled()) {
                    component->onScroll(event);
                }
            });
        }
    }

    void ComponentCollection::updateComponents(
        Profiler& profiler, float deltaTime) {
        flushNotStartedComponents();
        for (const auto& [type, data]: _components) {
            if (!data.first.onUpdate) continue;

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast
                    <AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([deltaTime](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled()) {
                    component->onUpdate(deltaTime);
                }
            });
        }
    }

    void ComponentCollection::lateUpdateComponents(
        Profiler& profiler, float deltaTime) {
        flushNotStartedComponents();
        for (const auto& [type, data]: _components) {
            if (!data.first.onLateUpdate) continue;

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast
                    <AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([deltaTime](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled()) {
                    component->onLateUpdate(deltaTime);
                }
            });
        }
    }


    void ComponentCollection::preDrawComponents(Profiler& profiler) {
        flushNotStartedComponents();
        for (const auto& [type, data]: _components) {
            if (!data.first.onPreDraw) continue;

            auto entry = ComponentRegister::instance().getEntry(type);
            auto name = entry.has_value() ? entry->name.c_str() : type.name();
            DEBUG_PROFILE_ID(profiler, type, name);

            auto ptr = std::static_pointer_cast
                    <AbstractClusteredLinkedCollection>(data.second);
            ptr->forEachRaw([](void* ptr) {
                auto* component = reinterpret_cast<Component*>(ptr);
                if (component->isEnabled()) {
                    component->onPreDraw();
                }
            });
        }
    }

    void ComponentCollection::callOnConstruction(void* rawComponent) {
        reinterpret_cast<Component*>(rawComponent)->onConstruction();
    }

    void ComponentCollection::flushNotStartedComponents() {
        for (; !_notStartedComponents.empty(); _notStartedComponents.pop()) {
            auto ptr = _notStartedComponents.front();

            if (ptr.isValid()) {
                ptr->onStart();
            }
        }
    }
}
