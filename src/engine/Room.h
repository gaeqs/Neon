//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_ROOM_H
#define RVTRACKING_ROOM_H

#include <engine/Camera.h>
#include <engine/ComponentsHolder.h>
#include <engine/GameObject.h>
#include <util/ClusteredLinkedCollection.h>

class Application;

class GameObject;

class Texture;

class Room {

    friend class Application;

    Application* _application;

    Camera _camera;
    ComponentsHolder _components;
    ClusteredLinkedCollection<GameObject> _gameObjects;

public:

    Room(const Room& other) = delete;

    Room();

    ~Room();

    Application* getApplication() const;

    const Camera& getCamera() const;

    Camera& getCamera();

    const ComponentsHolder& getComponents() const;

    ComponentsHolder& getComponents();

    GameObject* newGameObject();

    //region EVENTS

    void onResize();

    void update();

    void draw();


    //endregion

};


#endif //RVTRACKING_ROOM_H
