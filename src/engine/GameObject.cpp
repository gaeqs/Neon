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
    if(_room == nullptr) {
        throw std::runtime_error("Room is null!");
    }
}

GameObject::~GameObject() {
    for (const auto& item: _components) {
        if (item.isValid()) {
            item->destroy();
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

void GameObject::destroyComponent(IdentifiableWrapper<Component> component) {
    auto it = std::remove(_components.begin(), _components.end(),
                          component.raw());
    if (it != _components.end()) {
        getRoomComponents().destroyComponent(component);
    }
}
