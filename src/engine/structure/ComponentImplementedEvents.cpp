//
// Created by gaelr on 23/01/2023.
//

#include "ComponentImplementedEvents.h"

#include <engine/structure/Component.h>

ComponentImplementedEvents::ComponentImplementedEvents(
        std::type_index onConstructionFunction,
        std::type_index onStartFunction,
        std::type_index onUpdateFunction,
        std::type_index onLateUpdateFunction,
        std::type_index onKeyFunction,
        std::type_index onCursorMoveFunction) :
        onConstruction(
                onConstructionFunction != typeid(&Component::onConstruction)),
        onStart(onStartFunction != typeid(&Component::onStart)),
        onUpdate(onUpdateFunction != typeid(&Component::onUpdate)),
        onLateUpdate(onLateUpdateFunction != typeid(&Component::onLateUpdate)),
        onKey(onKeyFunction != typeid(&Component::onKey)),
        onCursorMove(onCursorMoveFunction != typeid(&Component::onCursorMove)) {
}
