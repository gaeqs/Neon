//
// Created by gaelr on 20/10/2022.
//

#include "ComponentCollection.h"

#include <engine/structure/Room.h>
#include <engine/structure/Component.h>
#include <engine/render/GraphicComponent.h>

ComponentCollection::ComponentCollection() :
        _components(),
        _notStartedComponents() {
}

void ComponentCollection::destroyComponent(
        const IdentifiableWrapper<Component>& component) {
    auto& type = typeid(*component.raw());
    auto it = _components.find(type);
    if (it == _components.end()) return;

    auto collection = std::reinterpret_pointer_cast<
            ClusteredLinkedCollection<Component>>(it->second.second);
    collection->remove(component.raw());
}

void ComponentCollection::invokeKeyEvent(Profiler& profiler,
                                         const KeyboardEvent& event) {
    flushNotStartedComponents();
    for (const auto& [type, data]: _components) {
        if (!data.first.onKey) continue;
        DEBUG_PROFILE_ID(profiler, type, type.name());
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

void ComponentCollection::invokeCursorMoveEvent(Profiler& profiler,
                                                const CursorMoveEvent& event) {
    flushNotStartedComponents();
    for (const auto& [type, data]: _components) {
        if (!data.first.onCursorMove) continue;
        DEBUG_PROFILE_ID(profiler, type, type.name());
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

void ComponentCollection::updateComponents(
        Profiler& profiler, float deltaTime) {
    flushNotStartedComponents();
    for (const auto& [type, data]: _components) {
        if (!data.first.onUpdate) continue;
        DEBUG_PROFILE_ID(profiler, type, type.name());
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
        DEBUG_PROFILE_ID(profiler, type, type.name());
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
        DEBUG_PROFILE_ID(profiler, type, type.name());
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

void ComponentCollection::test(std::type_index fun) {
    std::cout << fun.name() << std::endl;
}
