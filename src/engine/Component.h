//
// Created by gaelr on 19/10/2022.
//

#ifndef RVTRACKING_COMPONENT_H
#define RVTRACKING_COMPONENT_H

#include <cstdint>

#include <engine/GameObject.h>
#include <engine/Identifiable.h>
#include <engine/IdentifiableWrapper.h>

class KeyboardEvent;

class CursorMoveEvent;

/**
 * Represents a component inside a game object.
 * <p>
 * Components are represented by an identifier.
 * <p>
 * Components define behaviours to the attached game object.
 * You can add or remove components from a game object
 * using the appropriated methods.
 */
class Component : public Identifiable {

    friend class GameObject;

    template<class T> friend
    class IdentifiableWrapper;

    uint64_t _id;
    IdentifiableWrapper<GameObject> _gameObject;

public:

    Component(const Component& component) = delete;

    /**
     * THIS METHOD SHOULD ONLY BE USED BY ROOMS!
     * USERS MUSTN'T USE THIS METHOD.
     *
     * Creates a new component.
     */
    Component();

    /**
     * This game object's destructor.
     *
     * Children of this class may override
     * this destructor.
     */
    virtual ~Component() = default;

    /**
     * Returns the unique identifier of this component.
     * <p>
     * This value is immutable.
     *
     * @return the unique identifier.
     */
    [[nodiscard]] uint64_t getId() const override;

    /**
     * Returns a reference to this component's game object.
     * @return the game object.
     */
    [[nodiscard]] IdentifiableWrapper<GameObject> getGameObject() const;

    /**
     * Destroys this component.
     * <p>
     * All references to this component became invalid after this call.
     */
    void destroy();

    /**
     * Virtual method invoked when this component is constructed.
     */
    virtual void onConstruction();

    /**
     * Virtual method invoked before this component's first tick.
     */
    virtual void onStart();

    /**
     * Virtual method invoked every game tick.
     * @param deltaTime the time between the last and the current tick, in seconds.
     */
    virtual void onUpdate(float deltaTime);

    /**
     * Virtual method invoked when a key is pressed.
     * @param event the event.
     */
    virtual void onKey(const KeyboardEvent& event);

    /**
     * Virtual method invoked when the cursor is moved.
     * @param event the event.
     */
    virtual void onCursorMove(const CursorMoveEvent& event);

    // region UTIL

    /**
     * Returns the room of this component's game object.
     * <p>
     * This method is an alias of getGameObject()->getRoom().
     * @return the room.
     */
    [[nodiscard]] inline Room* getRoom() {
        return _gameObject->getRoom();
    }

    // endregion
};


#endif //RVTRACKING_COMPONENT_H
