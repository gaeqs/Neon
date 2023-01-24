//
// Created by gaelr on 23/01/2023.
//

#ifndef NEON_COMPONENTIMPLEMENTEDEVENTS_H
#define NEON_COMPONENTIMPLEMENTEDEVENTS_H

#include <typeindex>

/**
 * This class helps to determine which events
 * are implemented in a component.
 *
 * Not implemented events will be skipped.
 */
struct ComponentImplementedEvents {

    template<class T>
    static inline ComponentImplementedEvents fromGeneric() {
        return ComponentImplementedEvents(
                typeid(&T::onConstruction),
                typeid(&T::onStart),
                typeid(&T::onUpdate),
                typeid(&T::onLateUpdate),
                typeid(&T::onKey),
                typeid(&T::onCursorMove)
        );
    }

    bool onConstruction;
    bool onStart;
    bool onUpdate;
    bool onLateUpdate;
    bool onKey;
    bool onCursorMove;

    ComponentImplementedEvents(
            std::type_index onConstructionFunction,
            std::type_index onStartFunction,
            std::type_index onUpdateFunction,
            std::type_index onLateUpdateFunction,
            std::type_index onKeyFunction,
            std::type_index onCursorMoveFunction);

};


#endif //NEON_COMPONENTIMPLEMENTEDEVENTS_H
