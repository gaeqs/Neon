//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_APPLICATION_H
#define RVTRACKING_APPLICATION_H


#include <cstdint>
#include <string>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <util/Result.h>

class Room;

class Application {

    int32_t _width;
    int32_t _height;
    GLFWwindow* _window;

    std::shared_ptr<Room> _room;

public:

    Application(int32_t width, int32_t height);

    Result<GLFWwindow*, std::string> init(const std::string& name);

    Result<uint32_t, std::string> startGameLoop() const;

    int32_t getWidth() const;

    int32_t getHeight() const;

    float getAspectRatio() const;

    void setRoom(const std::shared_ptr<Room>& room);

    //region INTERNAL CALLS

    void internalForceSizeValues(int32_t width, int32_t height);

    void internalKeyEvent(int32_t key, int32_t scancode,
                          int32_t action, int32_t mods);

    //endregion
};


#endif //RVTRACKING_APPLICATION_H
