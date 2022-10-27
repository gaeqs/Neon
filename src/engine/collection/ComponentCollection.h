//
// Created by gaelr on 20/10/2022.
//

#ifndef RVTRACKING_COMPONENTCOLLECTION_H
#define RVTRACKING_COMPONENTCOLLECTION_H

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <queue>

#include <engine/IdentifiableWrapper.h>
#include <util/ClusteredLinkedCollection.h>

class Room;

class Component;

/**
 * This class holds all components inside a room.
 */
class ComponentCollection {

    std::unordered_map<std::type_index, std::shared_ptr<void>> _components;
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
    IdentifiableWrapper<T> newComponent(Args&& ... values) {
        auto it = _components.find(typeid(T));

        if (it == _components.end()) {
            auto pointer = std::make_shared<ClusteredLinkedCollection<T>>();
            _components.insert(std::make_pair(
                    std::type_index(typeid(T)),
                    std::static_pointer_cast<void>(pointer)
            ));

            T* component = pointer->emplace(values...);
            callOnConstruction(component);
            _notStartedComponents.push(static_cast<Component*>(component));
            return component;
        }

        auto components = std::static_pointer_cast
                <ClusteredLinkedCollection<T>>(it->second);

        T* component = components->emplace(values...);
        callOnConstruction(component);
        _notStartedComponents.push(static_cast<Component*>(component));
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
    std::shared_ptr<ClusteredLinkedCollection<T>> getComponentsOfType() const {
        auto it = _components.find(typeid(T));
        if (it == _components.end()) return nullptr;
        return std::static_pointer_cast<ClusteredLinkedCollection<T>>(
                it->second);
    }

    /**
     * Destroys the given component.
     *
     * All pointers to the given component will be invalid
     * after this call.
     *
     * @tparam T the type of the component.
     * @param component the component to be destroyed.
     */
    template<class T>
    void destroyComponent(const IdentifiableWrapper<T>& component) {
        auto ptr = getComponentsOfType<T>();
        if (ptr == nullptr) return;
        ptr->remove(component.raw());
    }

    /**
   * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
   * USERS MUSTN'T USE THIS METHOD.
   * <p>
   * Calls onKey() on all components.
   */
    void invokeKeyEvent(int32_t key, int32_t scancode,
                        int32_t action, int32_t mods);

    /**
     * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
     * USERS MUSTN'T USE THIS METHOD.
     * <p>
     * Calls onUpdate() on all components.
     * @param deltaTime the delay between this frame and the last one, in seconds.
     */
    void updateComponents(float deltaTime);

    /**
     * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
     * USERS MUSTN'T USE THIS METHOD.
     * <p>
     * Draws all graphic components.
     */
    void drawGraphicComponents(Room* room) const;
};


#endif //RVTRACKING_COMPONENTCOLLECTION_H
