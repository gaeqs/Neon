//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_GAMEOBJECT_H
#define RVTRACKING_GAMEOBJECT_H

#include <any>
#include "Room.h"

class Component;

class GameObject {

    uint64_t _id;
    Room* _room;

    std::vector<Component*> _components;

public:

    GameObject(const GameObject& other) = delete;

    explicit GameObject();

    uint64_t getId() const;

    Room* getRoom() const;

    template<class T>
    T& newComponent() {
        T* component = _room->newComponent<T>();
        component->_gameObject = this;
        _components.emplace_back(component);
    }


};


#endif //RVTRACKING_GAMEOBJECT_H
