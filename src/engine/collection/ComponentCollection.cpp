//
// Created by gaelr on 20/10/2022.
//

#include "ComponentCollection.h"

#include <engine/Room.h>
#include <engine/Renderer.h>
#include <engine/Component.h>
#include <engine/GraphicComponent.h>

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
            ClusteredLinkedCollection<Component>>(it->second);
    collection->remove(component.raw());
}

void ComponentCollection::invokeKeyEvent(const KeyboardEvent& event) {
    flushNotStartedComponents();
    for (const auto& item: _components) {
        auto ptr = std::static_pointer_cast
                <AbstractClusteredLinkedCollection>(item.second);
        ptr->forEachRaw([&event](void* ptr) {
            reinterpret_cast<Component*>(ptr)->
                    onKey(event);
        });
    }
}

void ComponentCollection::invokeCursorMoveEvent(const CursorMoveEvent& event) {
    flushNotStartedComponents();
    for (const auto& item: _components) {
        auto ptr = std::static_pointer_cast
                <AbstractClusteredLinkedCollection>(item.second);
        ptr->forEachRaw([&event](void* ptr) {
            reinterpret_cast<Component*>(ptr)->
                    onCursorMove(event);
        });
    }
}

void ComponentCollection::updateComponents(float deltaTime) {
    flushNotStartedComponents();
    for (const auto& item: _components) {
        auto ptr = std::static_pointer_cast
                <AbstractClusteredLinkedCollection>(item.second);
        ptr->forEachRaw([deltaTime](void* ptr) {
            reinterpret_cast<Component*>(ptr)->onUpdate(deltaTime);
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
