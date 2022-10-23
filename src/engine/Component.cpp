//
// Created by gaelr on 19/10/2022.
//

#include "Component.h"

#include <iostream>
#include <engine/GameObject.h>

uint64_t COMPONENT_ID_GENERATOR = 1;

Component::Component() : _id(COMPONENT_ID_GENERATOR++), _gameObject(nullptr) {
}

uint64_t Component::getId() const {
    return _id;
}

IdentifiableWrapper<GameObject> Component::getGameObject() const {
    return _gameObject;
}

void Component::destroy() {
    if (_gameObject.isValid()) {
        _gameObject->destroyComponent<Component>(this);
    }
}

void Component::onConstruction() {

}

void Component::onStart() {

}

void Component::onUpdate() {
}
