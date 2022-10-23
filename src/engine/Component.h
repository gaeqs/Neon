//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_COMPONENT_H
#define RVTRACKING_COMPONENT_H

#include <cstdint>

#include <engine/GameObject.h>
#include <engine/Identifiable.h>
#include <engine/IdentifiableWrapper.h>

class Component : public Identifiable {

    friend class GameObject;

    template<class T> friend
    class IdentifiableWrapper;

    uint64_t _id;
    IdentifiableWrapper<GameObject> _gameObject;

public:

    Component(const Component& component) = delete;

    Component();

    virtual ~Component() = default;

    uint64_t getId() const override;

    IdentifiableWrapper<GameObject> getGameObject() const;

    void destroy();

    virtual void onConstruction();

    virtual void onStart();

    virtual void onUpdate();
};


#endif //RVTRACKING_COMPONENT_H
