//
// Created by gaelr on 20/10/2022.
//

#ifndef RVTRACKING_COMPONENTSHOLDER_H
#define RVTRACKING_COMPONENTSHOLDER_H

#include <map>
#include <memory>
#include <typeindex>

#include <util/ClusteredLinkedCollection.h>

/**
 * This class holds all components inside a Room.
 */
class ComponentsHolder {

    std::map<std::type_index, std::shared_ptr<void>> _components;

public:

    /**
     * Creates the holder.
     */
    ComponentsHolder();

    /**
     *
     * THIS METHOD SHOULD ONLY BE USED BY GAME OBJECTS!
     * USERS MUSTN'T USE THIS METHOD.
     *
     * Creates a new component of type T.
     * @tparam T the type of the component.
     * @return a pointer to the new component.
     */
    template<class T>
    T* newComponent() {
        auto it = _components.find(typeid(T));

        if (it == _components.end()) {
            auto pointer = std::make_shared<ClusteredLinkedCollection<T>>();
            _components.insert(std::make_pair(
                    std::type_index(typeid(T)),
                    std::static_pointer_cast<void>(pointer)
            ));
            return pointer->emplace();
        }

        auto components = std::static_pointer_cast<ClusteredLinkedCollection<T>>(it->second);
        return components->emplace();
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
        return std::static_pointer_cast<ClusteredLinkedCollection<T>>(it->second);
    }

    /**
     * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
     * USERS MUSTN'T USE THIS METHOD.
     *
     * Calls onUpdate() on all components.
     */
    void updateComponents() const;

    /**
     * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
     * USERS MUSTN'T USE THIS METHOD.
     *
     * Draws all graphic components.
     */
    void drawGraphicComponents() const;
};


#endif //RVTRACKING_COMPONENTSHOLDER_H
