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

void ComponentCollection::updateComponents() {
    for (; !_notStartedComponents.empty(); _notStartedComponents.pop()) {
        auto ptr = _notStartedComponents.front();

        if (ptr.isValid()) {
            ptr->onStart();
        }
    }

    for (const auto& item: _components) {
        auto ptr = std::static_pointer_cast
                <AbstractClusteredLinkedCollection>(item.second);
        ptr->forEachRaw([](void* ptr) {
            reinterpret_cast<Component*>(ptr)->onUpdate();
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
