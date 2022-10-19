//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_COMPONENT_H
#define RVTRACKING_COMPONENT_H


#include <cstdint>

class Component {

    friend class GameObject;

    uint64_t _id;
    GameObject* _gameObject;

public:

    Component(const Component& component) = delete;

    Component();

    uint64_t getId() const;

};


#endif //RVTRACKING_COMPONENT_H
