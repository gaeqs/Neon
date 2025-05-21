//
// Created by gaelr on 23/01/2023.
//

#include "ComponentImplementedEvents.h"

#include <neon/structure/Component.h>

namespace neon
{
    ComponentImplementedEvents::ComponentImplementedEvents(
        std::type_index onConstructionFunction, std::type_index onStartFunction, std::type_index onUpdateFunction,
        std::type_index onLateUpdateFunction, std::type_index onPreDrawFunction, std::type_index onKeyFunction,
        std::type_index onCharFunction, std::type_index onMouseButtonFunction, std::type_index onCursorMoveFunction,
        std::type_index onScrollFunction) :
        onConstruction(onConstructionFunction != typeid(&Component::onConstruction)),
        onStart(onStartFunction != typeid(&Component::onStart)),
        onUpdate(onUpdateFunction != typeid(&Component::onUpdate)),
        onLateUpdate(onLateUpdateFunction != typeid(&Component::onLateUpdate)),
        onPreDraw(onPreDrawFunction != typeid(&Component::onPreDraw)),
        onKey(onKeyFunction != typeid(&Component::onKey)),
        onChar(onCharFunction != typeid(&Component::onChar)),
        onMouseButton(onMouseButtonFunction != typeid(&Component::onMouseButton)),
        onCursorMove(onCursorMoveFunction != typeid(&Component::onCursorMove)),
        onScroll(onScrollFunction != typeid(&Component::onScroll))
    {
    }
} // namespace neon
