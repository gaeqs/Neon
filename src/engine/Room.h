//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_ROOM_H
#define RVTRACKING_ROOM_H


#include "Camera.h"

class Application;

class Room {

    Camera _camera;
    Application* _application;

public:

    Room();

    Application* getApplication() const;

    const Camera& getCamera() const;

    Camera& getCamera();

    void bindApplication(Application* application);


    //region EVENTS

    void onResize();

    void update();

    void draw();


    //endregion

};


#endif //RVTRACKING_ROOM_H
