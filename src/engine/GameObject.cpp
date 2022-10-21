//
// Created by gaelr on 19/10/2022.
//

#include "GameObject.h"
#include <engine/Room.h>

uint64_t GAME_OBJECT_ID_GENERATOR = 0;

GameObject::GameObject(Room* room) : _id(GAME_OBJECT_ID_GENERATOR++), _room(room) {

}

uint64_t GameObject::getId() const {
    return _id;
}

Room* GameObject::getRoom() const {
    return _room;
}

ComponentsHolder& GameObject::getRoomComponents() const {
    return _room->getComponents();
}
