//
// Created by gaelr on 20/10/2022.
//

#include "ComponentCollection.h"

#include <engine/Room.h>
#include <engine/Renderer.h>
#include <engine/Component.h>
#include <engine/GraphicComponent.h>

ComponentCollection::ComponentCollection()
        : _components(),
          _notStartedComponents() {
}

void ComponentCollection::invokeKeyEvent(int32_t key, int32_t scancode,
                                         int32_t action, int32_t mods) {
    flushNotStartedComponents();
    for (const auto& item: _components) {
        auto ptr = std::static_pointer_cast
                <AbstractClusteredLinkedCollection>(item.second);
        ptr->forEachRaw([key, scancode, action, mods](void* ptr) {
            reinterpret_cast<Component*>(ptr)->
                    onKey(key, scancode, action, mods);
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

void ComponentCollection::drawGraphicComponents(Room* room) const {
    auto elements = getComponentsOfType<GraphicComponent>();
    auto renderer = room->getRenderer();
    if (renderer != nullptr) {
        renderer->render(room, elements);
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
