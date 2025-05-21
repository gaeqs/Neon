//
// Created by gaelr on 20/10/2022.
//

#ifndef RVTRACKING_COMPONENTCOLLECTION_H
#define RVTRACKING_COMPONENTCOLLECTION_H

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <queue>

#include <neon/structure/IdentifiableWrapper.h>
#include <neon/structure/ComponentImplementedEvents.h>
#include <neon/util/ClusteredLinkedCollection.h>
#include <neon/util/profile/Profiler.h>

namespace neon
{

    class Room;

    class Component;

    class GraphicComponent;

    struct KeyboardEvent;

    struct MouseButtonEvent;

    struct CursorMoveEvent;

    struct ScrollEvent;

    struct CharEvent;

    /**
     * This class holds all components inside a room.
     */
    class ComponentCollection
    {
        std::unordered_map<std::type_index, std::pair<ComponentImplementedEvents, std::shared_ptr<void>>> _components;
        std::queue<IdentifiableWrapper<Component>> _notStartedComponents;

        static void callOnConstruction(void* rawComponent);

        void flushNotStartedComponents();

      public:
        ComponentCollection(const ComponentCollection& other) = delete;

        /**
         * Creates the collection.
         */
        ComponentCollection();

        /**
         *
         * THIS METHOD SHOULD ONLY BE USED BY GAME OBJECTS!
         * USERS MUSTN'T USE THIS METHOD.
         *
         * Creates a new component of type T.
         * @tparam T the type of the component.
         * @tparam Args the types of the component's constructor's parameters.
         * @param values the parameters of the component's constructor.
         * @return a pointer to the new component.
         */
        template<class T, class... Args>
        IdentifiableWrapper<T> newComponent(Args&&... values)
        {
            auto it = _components.find(typeid(T));

            if (it == _components.end()) {
                auto events = ComponentImplementedEvents::fromGeneric<T>();
                auto pointer = std::make_shared<ClusteredLinkedCollection<T>>();
                _components.insert(std::make_pair(std::type_index(typeid(T)),
                                                  std::make_pair(events, std::static_pointer_cast<void>(pointer))));

                T* component = pointer->emplace(std::forward<Args>(values)...);
                _notStartedComponents.push(static_cast<Component*>(component));
                if (events.onConstruction) {
                    callOnConstruction(component);
                }
                return component;
            }

            auto components = std::static_pointer_cast<ClusteredLinkedCollection<T>>(it->second.second);

            T* component = components->emplace(std::forward<Args>(values)...);
            _notStartedComponents.push(static_cast<Component*>(component));
            if (it->second.first.onConstruction) {
                callOnConstruction(component);
            }
            return component;
        }

        /**
         * Returns a pointer to the ClusterLinkedCollection
         * with all components of type T.
         *
         * This pointer may be null if no components of type T were created.
         *
         * @tparam T the type of the component.
         * @return the pointer to the collection or nullptr.
         */
        template<class T>
        std::shared_ptr<ClusteredLinkedCollection<T>> getComponentsOfType() const
        {
            auto it = _components.find(typeid(T));
            if (it == _components.end()) {
                return nullptr;
            }
            return std::reinterpret_pointer_cast<ClusteredLinkedCollection<T>>(it->second.second);
        }

        /**
         * Destroys the given component.
         *
         * All pointers to the given component will be invalid
         * after this call.
         *
         * @param component the component to be destroyed.
         */
        void destroyComponent(const IdentifiableWrapper<Component>& component);

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         * <p>
         * Calls onKey() on all components.
         */
        void invokeKeyEvent(Profiler& profiler, const KeyboardEvent& event);

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         * <p>
         * Calls onChar() on all components.
         */
        void invokeCharEvent(Profiler& profiler, const CharEvent& event);

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         * <p>
         * Calls onMouseButton() on all components.
         */
        void invokeMouseButtonEvent(Profiler& profiler, const MouseButtonEvent& event);

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         * <p>
         * Calls onCursorMove() on all components.
         */
        void invokeCursorMoveEvent(Profiler& profiler, const CursorMoveEvent& event);

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         * <p>
         * Calls onScroll() on all components.
         */
        void invokeScrollEvent(Profiler& profiler, const ScrollEvent& event);

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         * <p>
         * Calls onUpdate() on all components.
         * @param profiler the profiler.
         * @param deltaTime the delay between this frame and the last one, in seconds.
         */
        void updateComponents(Profiler& profiler, float deltaTime);

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         * <p>
         * Calls onLateUpdate() on all components.
         * @param profiler the profiler.
         * @param deltaTime the delay between this frame and the last one, in seconds.
         */
        void lateUpdateComponents(Profiler& profiler, float deltaTime);

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         * <p>
         * Calls onPreDraw() on all components.
         * @param profiler the profiler.
         */
        void preDrawComponents(Profiler& profiler);
    };
} // namespace neon

#endif // RVTRACKING_COMPONENTCOLLECTION_H
