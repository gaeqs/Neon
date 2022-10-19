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

class Application;

class Room {

    friend class GameObject;

    Camera _camera;
    std::map<std::type_index, std::shared_ptr<void>> _components;
    std::vector<GameObject> _gameObjects;

    Application* _application;

    template<class T>
    T* newComponent() {
        auto it = _components.find(typeid(T));

        if (it == _components.end()) {
            auto pointer = std::make_shared<std::vector<T>>();
            _components.insert(std::make_pair(typeid(T), pointer));
            return &pointer->emplace_back();
        }

        auto* components = std::static_pointer_cast<std::vector<T>>(it->second);
        return &components->emplace_back();
    }

public:

    Room(const Room& other) = delete;

    Room();

    Application* getApplication() const;

    const Camera& getCamera() const;

    Camera& getCamera();

    void bindApplication(Application* application);

    GameObject& newGameObject();

    //region EVENTS

    void onResize();

    void update();

    void draw();


    //endregion

};


#endif //RVTRACKING_ROOM_H
