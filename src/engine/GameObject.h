//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_GAMEOBJECT_H
#define RVTRACKING_GAMEOBJECT_H

#include <any>
#include <vector>
#include "engine/collection/ComponentCollection.h"
#include <engine/IdentifiableWrapper.h>
#include <engine/Transform.h>
#include <engine/Identifiable.h>

class Room;

class Component;

class GameObject : public Identifiable {

    template<class T> friend
    class IdentifiableWrapper;

    uint64_t _id;
    Transform _transform;

    Room* _room;
    std::vector<IdentifiableWrapper<Component>> _components;

    ComponentCollection& getRoomComponents() const;

public:

    GameObject(const GameObject& other) = delete;

    explicit GameObject(Room* room);

    ~GameObject();

    uint64_t getId() const override;

    const Transform& getTransform() const;

    Transform& getTransform();

    Room* getRoom() const;

    void destroy();

    template<class T>
    IdentifiableWrapper<T> newComponent() {
        IdentifiableWrapper<T> component = getRoomComponents().newComponent<T>();
        component->_gameObject = this;
        auto raw = *reinterpret_cast<IdentifiableWrapper<Component>*>(&component);
        _components.emplace_back(raw);
        return component;
    }

    template<class T>
    void destroyComponent(IdentifiableWrapper<T> component) {
        auto unused = std::remove(_components.begin(), _components.end(),
                                  component.raw());
        getRoomComponents().destroyComponent(component);
    }

    template<class T>
    IdentifiableWrapper<T> findComponent() {
        for (const auto& item: _components) {
            if (dynamic_cast<T*>(item.raw())) {
                return item;
            }
        }
        return nullptr;
    }

};


#endif //RVTRACKING_GAMEOBJECT_H
