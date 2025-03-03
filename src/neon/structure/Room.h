//
// Created by grial on 19/10/22.
//

#ifndef NEON_ROOM_H
#define NEON_ROOM_H

#include <unordered_map>
#include <unordered_set>

#include <neon/geometry/Camera.h>

#include <neon/structure/collection/ComponentCollection.h>
#include <neon/structure/collection/IdentifiableCollection.h>
#include <neon/structure/GameObject.h>
#include <neon/render/Render.h>

#include <neon/util/ClusteredLinkedCollection.h>

namespace neon {
    class Application;

    class GameObject;

    class Component;

    struct KeyboardEvent;

    struct CursorMoveEvent;

    struct MouseButtonEvent;

    struct ScrollEvent;

    class Model;

    class Room {
        Application* _application;

        Camera _camera;
        ClusteredLinkedCollection<GameObject> _gameObjects;
        ComponentCollection _components;


        std::unordered_map<Model*, uint32_t> _usedModels;

        std::unordered_set<IdentifiableWrapper<Component>> _destroyLater;

    public:
        Room(const Room& other) = delete;

        Room(Application* application);

        ~Room();

        [[nodiscard]] Application* getApplication() const;

        [[nodiscard]] const Camera& getCamera() const;

        [[nodiscard]] Camera& getCamera();

        [[nodiscard]] const ComponentCollection& getComponents() const;

        [[nodiscard]] ComponentCollection& getComponents();

        IdentifiableWrapper<GameObject> newGameObject();

        void destroyGameObject(IdentifiableWrapper<GameObject> gameObject);

        void destroyComponentLater(IdentifiableWrapper<Component> component);

        size_t getGameObjectAmount();

        void forEachGameObject(std::function<void(GameObject*)> consumer);

        void forEachGameObject(
            std::function<void(const GameObject*)> consumer) const;

        // region INTERNAL

        const std::unordered_map<Model*, uint32_t>& usedModels() const;

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
        void unmarkUsingModel(Model* model);

        size_t loadGameObjects(nlohmann::json json, AssetLoaderContext context);

        size_t loadGameObjects(const std::filesystem::path& path, AssetLoaderContext context);

        // endregion

        //region EVENTS

        void onKey(const KeyboardEvent& event);

        void onMouseButton(const MouseButtonEvent& event);

        void onCursorMove(const CursorMoveEvent& event);

        void onScroll(const ScrollEvent& event);

        void update(float deltaTime);

        void preDraw();


        //endregion
    };
}


#endif //NEON_ROOM_H
