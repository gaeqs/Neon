//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_ROOM_H
#define RVTRACKING_ROOM_H


#include <map>
#include <any>
#include <typeindex>
#include <memory>
#include <vector>

#include "Camera.h"
#include "../util/ClusteredLinkedCollection.h"

class Application;

class GameObject;

class Room {

    friend class Application;

    friend class GameObject;

    Camera _camera;
    std::map<std::type_index, std::shared_ptr<void>> _components;

    // Pointer fixes cross-reference error.
    ClusteredLinkedCollection<GameObject>* _gameObjects;

    Application* _application;

    template<class T>
    T* newComponent() {
        auto it = _components.find(typeid(T));

        if (it == _components.end()) {
            auto pointer = std::make_shared<ClusteredLinkedCollection<T>>();
            _components.insert(std::make_pair(
                    std::type_index(typeid(T)),
                    std::static_pointer_cast<void>(pointer)
            ));
            return pointer->emplace();
        }

        auto components = std::static_pointer_cast<ClusteredLinkedCollection<T>>(it->second);
        return components->emplace();
    }

public:

    Room(const Room& other) = delete;

    Room();

    ~Room();

    Application* getApplication() const;

    const Camera& getCamera() const;

    Camera& getCamera();

    GameObject* newGameObject();

    //region EVENTS

    void onResize();

    void update();

    void draw();


    //endregion

};


#endif //RVTRACKING_ROOM_H
