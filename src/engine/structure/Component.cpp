//
// Created by gaelr on 19/10/2022.
//

#include "Component.h"

#include <iostream>
#include "GameObject.h"

uint64_t COMPONENT_ID_GENERATOR = 1;

Component::Component() :
        _id(COMPONENT_ID_GENERATOR++),
        _enabled(true),
        _gameObject(nullptr) {
}

uint64_t Component::getId() const {
    return _id;
}

IdentifiableWrapper<GameObject> Component::getGameObject() const {
    return _gameObject;
}

bool Component::isEnabled() const {
    return _enabled;
}

void Component::setEnabled(bool enabled) {
    _enabled = enabled;
}

void Component::destroy() {
    if (_gameObject.isValid()) {
        _gameObject->destroyComponent(this);
    }
}

void Component::onConstruction() {

}

void Component::onStart() {

}

void Component::onUpdate(float deltaTime) {
}

void Component::onLateUpdate(float deltaTime) {
}

void Component::onPreDraw() {
}

void Component::onKey(const KeyboardEvent& event) {
}

void Component::onCursorMove(const CursorMoveEvent& event) {
}

void Component::drawEditor() {
}
