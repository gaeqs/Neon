//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_GAMEOBJECT_H
#define RVTRACKING_GAMEOBJECT_H

#include <any>
#include <vector>
#include "ComponentsHolder.h"

class Room;

class Component;

class GameObject {

    uint64_t _id;
    Room* _room;

    std::vector<Component*> _components;

    ComponentsHolder& getRoomComponents() const;

public:

    GameObject(const GameObject& other) = delete;

    explicit GameObject(Room* room);

    uint64_t getId() const;

    Room* getRoom() const;

    template<class T>
    T* newComponent() {
        T* component = getRoomComponents().newComponent<T>();
        component->_gameObject = this;
        _components.emplace_back(component);
        return component;
    }


};


#endif //RVTRACKING_GAMEOBJECT_H
