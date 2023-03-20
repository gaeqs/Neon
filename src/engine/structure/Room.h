//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_ROOM_H
#define RVTRACKING_ROOM_H

#include <unordered_map>

#include <engine/geometry/Camera.h>

#include <engine/structure/collection/ComponentCollection.h>
#include <engine/structure/collection/IdentifiableCollection.h>
#include <engine/shader/ShaderUniformBuffer.h>
#include <engine/shader/ShaderUniformDescriptor.h>
#include <engine/render/Render.h>

#include "util/ClusteredLinkedCollection.h"

namespace neon {

    class Application;

    class GameObject;

    class Component;

    class KeyboardEvent;

    class CursorMoveEvent;

    class Model;

    class Room {

        Application* _application;

        Camera _camera;
        ClusteredLinkedCollection<GameObject> _gameObjects;
        ComponentCollection _components;

        std::shared_ptr<ShaderUniformDescriptor> _globalUniformDescriptor;
        ShaderUniformBuffer _globalUniformBuffer;

        std::unordered_map<Model*, uint32_t> _usedModels;

        Render _render;

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

        [[nodiscard]] const std::shared_ptr<ShaderUniformDescriptor>&
        getGlobalUniformDescriptor() const;

        [[nodiscard]] const ShaderUniformBuffer& getGlobalUniformBuffer() const;

        [[nodiscard]] ShaderUniformBuffer& getGlobalUniformBuffer();

        [[nodiscard]] const Render& getRender() const;

        [[nodiscard]] Render& getRender();

        IdentifiableWrapper<GameObject> newGameObject();

        void destroyGameObject(IdentifiableWrapper<GameObject> gameObject);

        size_t getGameObjectAmount();

        void forEachGameObject(std::function<void(GameObject*)> consumer);

        void forEachGameObject(
                std::function<void(const GameObject*)> consumer) const;

        // region INTERNAL

        /**
         * THIS METHOD SHOULD ONLY BE USED BY GRAPHIC COMPONENTS!
         * USERS MUSTN'T USE THIS METHOD.
         * <p>
         * Marks the given model as being used to render the scene.
         */
        void markUsingModel(Model* model);

        /**
         * THIS METHOD SHOULD ONLY BE USED BY GRAPHIC COMPONENTS!
         * USERS MUSTN'T USE THIS METHOD.
         * <p>
         * Unmarks the given model as being used to render the scene.
         */
        void unmarkUsingModel (Model* model);

        // endregion

        //region EVENTS

        void onKey(const KeyboardEvent& event);

        void onCursorMove(const CursorMoveEvent& event);

        void update(float deltaTime);

        void preDraw();

        void draw();


        //endregion

    };
}


#endif //RVTRACKING_ROOM_H
