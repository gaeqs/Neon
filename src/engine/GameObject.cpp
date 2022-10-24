//
// Created by gaelr on 19/10/2022.
//

#include "GameObject.h"
#include <engine/Room.h>
#include <engine/Component.h>

uint64_t GAME_OBJECT_ID_GENERATOR = 1;

GameObject::GameObject(Room* room) : _id(GAME_OBJECT_ID_GENERATOR++),
                                     _transform(),
                                     _room(room),
                                     _components() {
}

GameObject::~GameObject() {
    for (const auto& item: _components) {
        if (item.isValid()) {
            _room->getComponents().destroyComponent(item);
        }
    }
}

uint64_t GameObject::getId() const {
    return _id;
}

const Transform& GameObject::getTransform() const {
    return _transform;
}

Transform& GameObject::getTransform() {
    return _transform;
}

Room* GameObject::getRoom() const {
    return _room;
}

void GameObject::destroy() {
    _room->destroyGameObject(this);
}

ComponentCollection& GameObject::getRoomComponents() const {
    return _room->getComponents();
}