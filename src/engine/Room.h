//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_ROOM_H
#define RVTRACKING_ROOM_H

#include <engine/Camera.h>
#include <engine/GameObject.h>

#include <engine/collection/ComponentCollection.h>
#include <engine/collection/IdentifiableCollection.h>
#include <engine/collection/TextureCollection.h>
#include <engine/shader/ShaderUniformBuffer.h>
#include <engine/shader/ShaderUniformDescriptor.h>
#include <engine/shader/ShaderProgram.h>
#include <engine/model/Model.h>

#include <util/ClusteredLinkedCollection.h>

class Application;

class GameObject;

class Renderer;

class KeyboardEvent;

class CursorMoveEvent;

class Room {

    Application* _application;

    Camera _camera;
    ClusteredLinkedCollection<GameObject> _gameObjects;
    ComponentCollection _components;
    TextureCollection _textures;
    IdentifiableCollection<Model> _models;
    IdentifiableCollection<ShaderProgram> _shaders;

    std::shared_ptr<ShaderUniformDescriptor> _globalUniformDescriptor;
    ShaderUniformBuffer _globalUniformBuffer;

    std::shared_ptr<Renderer> _renderer;

public:

    Room(const Room& other) = delete;

    Room(Application* application,
         const std::shared_ptr<ShaderUniformDescriptor>& descriptor);

    ~Room();

    [[nodiscard]] Application* getApplication() const;

    [[nodiscard]] const Camera& getCamera() const;

    [[nodiscard]] Camera& getCamera();

    [[nodiscard]] const ComponentCollection& getComponents() const;

    [[nodiscard]] ComponentCollection& getComponents();

    [[nodiscard]] const TextureCollection& getTextures() const;

    [[nodiscard]] TextureCollection& getTextures();

    [[nodiscard]] const IdentifiableCollection<Model>& getModels() const;

    [[nodiscard]] IdentifiableCollection<Model>& getModels();

    [[nodiscard]] const std::shared_ptr<ShaderUniformDescriptor>&
    getGlobalUniformDescriptor() const;

    [[nodiscard]] const ShaderUniformBuffer& getGlobalUniformBuffer() const;

    [[nodiscard]] ShaderUniformBuffer& getGlobalUniformBuffer();

    [[nodiscard]] const IdentifiableCollection<ShaderProgram>&
    getShaders() const;

    [[nodiscard]] IdentifiableCollection<ShaderProgram>& getShaders();

    [[nodiscard]] const std::shared_ptr<Renderer>& getRenderer() const;

    [[nodiscard]] std::shared_ptr<Renderer>& getRenderer();

    void setRenderer(const std::shared_ptr<Renderer>& renderer);

    [[nodiscard]] IdentifiableWrapper<GameObject> newGameObject();

    void destroyGameObject(IdentifiableWrapper<GameObject> gameObject);

    //region EVENTS

    void onResize();

    void onKey(const KeyboardEvent& event);

    void onCursorMove(const CursorMoveEvent& event);

    void update(float deltaTime);

    void draw();


    //endregion

};


#endif //RVTRACKING_ROOM_H
