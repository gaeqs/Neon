//
// Created by gaelr on 19/10/2022.
//

#include "GameObject.h"

#include "Room.h"
#include "Component.h"

namespace neon {
    uint64_t GAME_OBJECT_ID_GENERATOR = 1;

    GameObject::GameObject(Room *room) :
            _id(GAME_OBJECT_ID_GENERATOR++),
            _name("Game Object " + std::to_string(_id)),
            _transform(this),
            _room(room),
            _components(),
            _parent(nullptr),
            _children() {
        if (_room == nullptr) {
            throw std::runtime_error("Room is null!");
        }
    }

    GameObject::~GameObject() {
        auto copy = _components; // Avoid concurrent exceptions!
        for (const auto &item: copy) {
            if (item.isValid()) {
                item->destroy();
            }
        }
        if (_parent) {
            _parent->_children.erase(IdentifiableWrapper<GameObject>(this));
        }
    }

    uint64_t GameObject::getId() const {
        return _id;
    }

    const std::string &GameObject::getName() const {
        return _name;
    }

    void GameObject::setName(const std::string &name) {
        _name = name;
    }

    const Transform &GameObject::getTransform() const {
        return _transform;
    }

    Transform &GameObject::getTransform() {
        return _transform;
    }

    Room *GameObject::getRoom() const {
        return _room;
    }

    void GameObject::destroy() {
        _room->destroyGameObject(this);
    }

    IdentifiableWrapper<GameObject> GameObject::getParent() const {
        return _parent;
    }

    void GameObject::setParent(const IdentifiableWrapper<GameObject> &parent) {
        if (_parent) {
            _parent->_children.erase(IdentifiableWrapper<GameObject>(this));
        }

        _parent = parent;

        if (_parent) {
            _parent->_children.insert(IdentifiableWrapper<GameObject>(this));
        }
    }

    const std::unordered_set<IdentifiableWrapper<GameObject>> &
    GameObject::getChildren() const {
        return _children;
    }

    const std::unordered_set<IdentifiableWrapper<Component>> &
    GameObject::getComponents() const {
        return _components;
    }

    ComponentCollection &GameObject::getRoomComponents() const {
        return _room->getComponents();
    }

    void GameObject::destroyComponent(IdentifiableWrapper<Component> component) {
        auto amount = _components.erase(component);
        if (amount) {
            getRoomComponents().destroyComponent(component);
        }
    }
}