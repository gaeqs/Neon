//
// Created by gaelr on 20/10/2022.
//

#include "ComponentCollection.h"

#include "engine/Component.h"
#include "engine/GraphicComponent.h"

ComponentCollection::ComponentCollection()
        : _components() {
}

void ComponentCollection::updateComponents() const {
    for (const auto& item: _components) {
        auto ptr = std::static_pointer_cast
                <AbstractClusteredLinkedCollection>(item.second);
        ptr->forEachRaw([](void* ptr) {
            reinterpret_cast<Component*>(ptr)->onUpdate();
        });
    }
}

void ComponentCollection::drawGraphicComponents() const {
    auto ptr = getComponentsOfType<GraphicComponent>();
    if (ptr != nullptr) {
        ptr->forEach([](GraphicComponent* component) {
            // TODO DRAW
        });
    }
}
