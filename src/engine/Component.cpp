//
// Created by gaelr on 19/10/2022.
//

#include "Component.h"

#include <iostream>
#include <engine/GameObject.h>

uint64_t COMPONENT_ID_GENERATOR = 0;

Component::Component() : _id(COMPONENT_ID_GENERATOR++) {
}

uint64_t Component::getId() const {
    return _id;
}

GameObject* Component::getGameObject() const {
    return _gameObject;
}

void Component::onConstruction() {

}

void Component::onStart() {

}

void Component::onUpdate() {
}
