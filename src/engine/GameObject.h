//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_GAMEOBJECT_H
#define RVTRACKING_GAMEOBJECT_H

#include <any>
#include <vector>
#include <engine/ComponentsHolder.h>
#include <engine/ComponentWrapper.h>

class Room;

class Component;

class GameObject {

    uint64_t _id;
    Room* _room;

    std::vector<ComponentWrapper<Component>> _components;

    ComponentsHolder& getRoomComponents() const;

public:

    GameObject(const GameObject& other) = delete;

    explicit GameObject(Room* room);

    uint64_t getId() const;

    Room* getRoom() const;

    template<class T>
    ComponentWrapper<T> newComponent() {
        ComponentWrapper<T> component = getRoomComponents().newComponent<T>();
        component->_gameObject = this;
        auto raw = *reinterpret_cast<ComponentWrapper<Component>*>(&component);
        _components.emplace_back(raw);
        return component;
    }

    template<class T>
    void removeComponent (ComponentWrapper<T> component) {
        getRoomComponents().removeComponent(component);
    }


};


#endif //RVTRACKING_GAMEOBJECT_H
