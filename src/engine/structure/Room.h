//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_ROOM_H
#define RVTRACKING_ROOM_H

#include <engine/geometry/Camera.h>

#include <engine/structure/collection/ComponentCollection.h>
#include <engine/structure/collection/IdentifiableCollection.h>
#include <engine/structure/collection/TextureCollection.h>
#include <engine/shader/ShaderUniformBuffer.h>
#include <engine/shader/ShaderUniformDescriptor.h>
#include <engine/shader/ShaderProgram.h>
#include <engine/shader/Material.h>
#include <engine/model/Model.h>
#include <engine/render/Render.h>

#include "util/ClusteredLinkedCollection.h"

namespace neon {

    class Application;

    class GameObject;

    class Component;

    class KeyboardEvent;

    class CursorMoveEvent;

    class Room {

        Application *_application;

        Camera _camera;
        ClusteredLinkedCollection<GameObject> _gameObjects;
        ComponentCollection _components;
        TextureCollection _textures;
        IdentifiableCollection<ShaderProgram> _shaders;
        IdentifiableCollection<Material> _materials;

        std::shared_ptr<ShaderUniformDescriptor> _globalUniformDescriptor;
        ShaderUniformBuffer _globalUniformBuffer;

        Render _render;

    public:

        Room(const Room &other) = delete;

        Room(Application *application,
             const std::shared_ptr<ShaderUniformDescriptor> &descriptor);

        ~Room();

        [[nodiscard]] Application *getApplication() const;

        [[nodiscard]] const Camera &getCamera() const;

        [[nodiscard]] Camera &getCamera();

        [[nodiscard]] const ComponentCollection &getComponents() const;

        [[nodiscard]] ComponentCollection &getComponents();

        [[nodiscard]] const TextureCollection &getTextures() const;

        [[nodiscard]] TextureCollection &getTextures();

        [[nodiscard]] const std::shared_ptr<ShaderUniformDescriptor> &
        getGlobalUniformDescriptor() const;

        [[nodiscard]] const ShaderUniformBuffer &getGlobalUniformBuffer() const;

        [[nodiscard]] ShaderUniformBuffer &getGlobalUniformBuffer();

        [[nodiscard]] const IdentifiableCollection<ShaderProgram> &
        getShaders() const;

        [[nodiscard]] IdentifiableCollection<ShaderProgram> &getShaders();

        [[nodiscard]] const IdentifiableCollection<Material> &getMaterials() const;

        [[nodiscard]] IdentifiableCollection<Material> &getMaterials();

        [[nodiscard]] const Render &getRender() const;

        [[nodiscard]] Render &getRender();

        IdentifiableWrapper<GameObject> newGameObject();

        void destroyGameObject(IdentifiableWrapper<GameObject> gameObject);

        size_t getGameObjectAmount();

        void forEachGameObject(std::function<void(GameObject *)> consumer);

        void forEachGameObject(
                std::function<void(const GameObject *)> consumer) const;

        //region EVENTS

        void onKey(const KeyboardEvent &event);

        void onCursorMove(const CursorMoveEvent &event);

        void update(float deltaTime);

        void preDraw();

        void draw();


        //endregion

    };
}


#endif //RVTRACKING_ROOM_H
