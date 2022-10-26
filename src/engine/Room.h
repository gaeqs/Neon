//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_ROOM_H
#define RVTRACKING_ROOM_H

#include <engine/Camera.h>
#include <engine/GameObject.h>

#include <engine/collection/ComponentCollection.h>
#include <engine/collection/TextureCollection.h>
#include <engine/collection/ModelCollection.h>

#include <util/ClusteredLinkedCollection.h>

class Application;

class GameObject;

class Renderer;

class Room {

    friend class Application;

    Application* _application;

    Camera _camera;
    ClusteredLinkedCollection<GameObject> _gameObjects;
    ComponentCollection _components;
    TextureCollection _textures;
    ModelCollection _models;

    std::shared_ptr<Renderer> _renderer;

public:

    Room(const Room& other) = delete;

    Room();

    ~Room();

    Application* getApplication() const;

    const Camera& getCamera() const;

    Camera& getCamera();

    const ComponentCollection& getComponents() const;

    ComponentCollection& getComponents();

    const TextureCollection& getTextures() const;

    TextureCollection& getTextures();

    const ModelCollection& getModels() const;

    ModelCollection& getModels();

    const std::shared_ptr<Renderer>& getRenderer() const;

    std::shared_ptr<Renderer>& getRenderer();

    void setRenderer(const std::shared_ptr<Renderer>& renderer);

    IdentifiableWrapper<GameObject> newGameObject();

    void destroyGameObject(IdentifiableWrapper<GameObject> gameObject);

    //region EVENTS

    void onResize();

    void onKey(int32_t key, int32_t scancode, int32_t action, int32_t mods);

    void update(float deltaTime);

    void draw();


    //endregion

};


#endif //RVTRACKING_ROOM_H
