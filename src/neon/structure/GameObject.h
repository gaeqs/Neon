//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_GAMEOBJECT_H
#define RVTRACKING_GAMEOBJECT_H

#include <any>
#include <vector>
#include <unordered_set>

#include <neon/geometry/Transform.h>
#include <neon/structure/collection/ComponentCollection.h>
#include <neon/structure/IdentifiableWrapper.h>
#include <neon/structure/Identifiable.h>

namespace neon
{
    class Room;

    class Component;

    /**
     * Represents an object inside a scene.
     * <p>
     * Objects are represented by an identifier, a transform
     * and a collection of components.
     * <p>
     * Components define the behaviors of the game object.
     * You can add or remove components from a game object
     * using the appropriated methods.
     */
    class GameObject : public Identifiable
    {
        template<class T>
        friend class IdentifiableWrapper;

        uint64_t _id;
        std::string _name;

        Transform _transform;

        Room* _room;
        std::unordered_set<IdentifiableWrapper<Component>> _components;

        // Hierarchy
        IdentifiableWrapper<GameObject> _parent;
        std::unordered_set<IdentifiableWrapper<GameObject>> _children;

        /**
         * Helper method that returns the component's collection
         * of this game object's room.
         *
         * Thanks to this method we avoid including Room.h on this header.
         *
         * @return the collection.
         */
        [[nodiscard]] ComponentCollection& getRoomComponents() const;

      public:
        GameObject(const GameObject& other) = delete;

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         *
         * Creates a new game object.
         * @param room the room this game object is inside.
         */
        explicit GameObject(Room* room);

        /**
         * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
         * USERS MUSTN'T USE THIS METHOD.
         *
         * Creates a new game object.
         * @param room the room this game object is inside.
         * @param name the name of the game object.
         */
        explicit GameObject(Room* room, std::string name);

        /**
         * This game object's destructor.
         *
         * All components inside this game object
         * are destroyed when this game object is destroyed.
         *
         */
        ~GameObject() override;

        /**
         * Returns the unique identifier of this game object.
         * <p>
         * This value is immutable.
         *
         * @return the unique identifier.
         */
        [[nodiscard]] uint64_t getId() const override;

        /**
         * Returns the name of this game object.
         *
         * @return the name.
         */
        const std::string& getName() const;

        /**
         * Sets the name of this game object.
         *
         * @param name the name.
         */
        void setName(const std::string& name);

        /**
         * Returns the transformation of this game object.
         * @return the transformation.
         */
        [[nodiscard]] const Transform& getTransform() const;

        /**
         * Returns the transformation of this game object.
         * @return the transformation.
         */
        [[nodiscard]] Transform& getTransform();

        /**
         * Returns the room this game object is inside of.
         * @return the room.
         */
        [[nodiscard]] Room* getRoom() const;

        /**
         * Destroys this game object.
         * <p>
         * All references to this game object became invalid after
         * this call.
         */
        void destroy();

        /**
         * Returns the parent of this game object. The parent
         * may be null.
         * <p>
         * The parent defines the origin
         * of this game object's coordinate system.
         * When the parent moves, this game object moves along
         * with the parent.
         * <p>
         * This method is an alias of getTransform().getParent().
         *
         * @return the parent or null.
         */
        [[nodiscard]] IdentifiableWrapper<GameObject> getParent() const;

        /**
         * Sets the parent of this game object. The parent
         * may be null.
         * <p>
         * The parent defines the origin
         * of this game object's coordinate system.
         * When the parent moves, this game object moves along
         * with the parent.
         * <p>
         * This method is an alias of getTransform().setParent().
         *
         * @param parent the parent or null.
         */
        void setParent(const IdentifiableWrapper<GameObject>& parent);

        /**
         * Returns the collection of children of this game object.
         *
         * This collection cannot be modified directly:
         * use setParent on the children to modify this collection.
         *
         * @return the collection of children.
         */
        [[nodiscard]] const std::unordered_set<IdentifiableWrapper<GameObject>>& getChildren() const;

        /**
         * Returns a collection with all components inside this
         * game object.
         *
         * This collection is not a copy and may be modified
         * internally after this method's call.
         *
         * This collection <b>never</b> contains null elements.
         *
         * @return the components.
         */
        [[nodiscard]] const std::unordered_set<IdentifiableWrapper<Component>>& getComponents() const;

        /**
         * Creates a new component, attaching it to this game object.
         * @tparam T the type of the component.
         * @tparam Args the types of the arguments of the component's constructor.
         * @param values the  arguments of the component's constructor.
         * @return a reference pointing to the new component.
         */
        template<class T, class... Args>
            requires std::is_base_of_v<Component, T>
        IdentifiableWrapper<T> newComponent(Args&&... values)
        {
            IdentifiableWrapper<T> component = getRoomComponents().newComponent<T>(std::forward<Args>(values)...);
            component->_gameObject = this;
            auto raw = *reinterpret_cast<IdentifiableWrapper<Component>*>(&component);
            _components.insert(raw);
            return component;
        }

        /**
         * Destroys the given component.
         * The component must belong to this game object for
         * this operation to work.
         * <p>
         * Using this method directly is not recommended. Use
         * Component#destroy() instead.
         * @param component the component.
         */
        void destroyComponent(IdentifiableWrapper<Component> component);

        /**
         * Returns the first component inside this game object
         * that matches the given type.
         * <p>
         * If the component cannot be found, this method returns null.
         * <p>
         * This method may return a component whose type is a child
         * of the given type.
         *
         * @tparam T the type of the component.
         * @return the component or null.
         */
        template<class T>
            requires std::is_base_of_v<Component, T>
        [[nodiscard]] IdentifiableWrapper<T> findComponent()
        {
            for (const auto& item : _components) {
                if (auto t = dynamic_cast<T*>(item.raw())) {
                    return t;
                }
            }
            return nullptr;
        }

        /**
         * Returns the first component of type T that is found in this game object or any of its children recursively.
         * <p>
         * This performs a depth-first search.
         *
         * @tparam T The type of the component to find.
         * @return A wrapper to the component, or null if not found.
         */
        template<class T>
            requires std::is_base_of_v<Component, T>
        [[nodiscard]] IdentifiableWrapper<T> findComponentInChildren()
        {
            auto component = findComponent<T>();
            if (component != nullptr) {
                return component;
            }
            for (const auto& children : _children) {
                component = children->findComponentInChildren<T>();
                if (component != nullptr) {
                    return component;
                }
            }

            return nullptr;
        }

        /**
         * Returns the first component of type T that is found in this game object or any of its parents recursively.
         *
         * @tparam T The type of the component to find.
         * @return A wrapper to the component, or null if not found.
         */
        template<class T>
            requires std::is_base_of_v<Component, T>
        [[nodiscard]] IdentifiableWrapper<T> findComponentInParent()
        {
            auto component = findComponent<T>();
            if (component != nullptr) {
                return component;
            }

            if (_parent != nullptr) {
                return _parent->findComponentInParent<T>();
            }

            return nullptr;
        }

        /**
         * Returns a list containing all components inside this game object
         * that match the given type.
         * <p>
         * This method may return components whose types are children
         * of the given type.
         *
         * @tparam T the type of the component.
         * @return the list.
         */
        template<class T>
            requires std::is_base_of_v<Component, T>
        [[nodiscard]] std::vector<IdentifiableWrapper<T>> findComponents()
        {
            std::vector<IdentifiableWrapper<T>> list;
            for (const auto& item : _components) {
                if (auto d = dynamic_cast<T*>(item.raw())) {
                    list.push_back(d);
                }
            }
            return list;
        }

        /**
         * Returns all components of type T that are found in this game object and all its children recursively.
         *
         * @tparam T The type of the component to find.
         * @return A list with all the found components.
         */
        template<class T>
            requires std::is_base_of_v<Component, T>
        [[nodiscard]] std::vector<IdentifiableWrapper<T>> findComponentsInChildren()
        {
            auto components = findComponents<T>();
            for (const auto& children : _children) {
                auto childrenComponents = children->findComponentsInChildren<T>();
                components.reserve(components.size() + childrenComponents.size());
                components.insert(components.end(), childrenComponents.begin(), childrenComponents.end());
            }

            return components;
        }

        /**
         * Returns all components of type T that are found in this game object and all its parents recursively.
         *
         * @tparam T The type of the component to find.
         * @return A list with all the found components.
         */
        template<class T>
            requires std::is_base_of_v<Component, T>
        [[nodiscard]] std::vector<IdentifiableWrapper<T>> findComponentsInParent()
        {
            auto components = findComponents<T>();
            if (_parent != nullptr) {
                auto parentComponents = _parent->findComponentsInParent<T>();
                components.reserve(components.size() + parentComponents.size());
                components.insert(components.end(), parentComponents.begin(), parentComponents.end());
            }

            return components;
        }
    };
} // namespace neon

#endif // RVTRACKING_GAMEOBJECT_H
