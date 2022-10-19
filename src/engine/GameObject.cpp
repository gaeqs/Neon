//
// Created by gaelr on 19/10/2022.
//

#include "GameObject.h"

uint64_t GAME_OBJECT_ID_GENERATOR = 0;

GameObject::GameObject() : _id(GAME_OBJECT_ID_GENERATOR++), _room(nullptr) {

}

uint64_t GameObject::getId() const {
    return _id;
}

Room* GameObject::getRoom() const {
    return _room;
}
