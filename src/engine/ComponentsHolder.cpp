//
// Created by gaelr on 20/10/2022.
//

#include "ComponentsHolder.h"

#include "Component.h"
#include "GraphicComponent.h"

ComponentsHolder::ComponentsHolder()
        : _components() {
}

void ComponentsHolder::updateComponents() const {
    for (const auto& item: _components) {
        auto ptr = std::static_pointer_cast<AbstractClusteredLinkedCollection>(item.second);
        ptr->forEachRaw([](void* ptr) {
            reinterpret_cast<Component*>(ptr)->onUpdate();
        });
    }
}

void ComponentsHolder::drawGraphicComponents() const {
    auto ptr = getComponentsOfType<GraphicComponent>();
    if (ptr != nullptr) {
        ptr->forEach([](GraphicComponent* component) {
            // TODO DRAW
        });
    }
}
