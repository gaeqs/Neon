//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_APPLICATION_H
#define RVTRACKING_APPLICATION_H


#include <cstdint>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <util/Result.h>

#ifdef USE_OPENGL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gl/GLApplication.h>

#endif
#ifdef USE_VULKAN

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/VKApplication.h>

#endif


class Room;

class Application {

public:

#ifdef USE_OPENGL
    using Implementation = GLApplication;
#endif
#ifdef USE_VULKAN
    using Implementation = VKApplication;
#endif

protected:

    int32_t _width;
    int32_t _height;
    GLFWwindow* _window;

    std::shared_ptr<Room> _room;

    glm::dvec2 _last_cursor_pos;

    Implementation _implementation;

public:

    Application(int32_t width, int32_t height);

    ~Application();

    Result<GLFWwindow*, std::string> init(const std::string& name);

    [[nodiscard]] Result<uint32_t, std::string> startGameLoop();

    [[nodiscard]] const Implementation& getImplementation() const;

    [[nodiscard]] Implementation& getImplementation();

    [[nodiscard]] int32_t getWidth() const;

    [[nodiscard]] int32_t getHeight() const;

    [[nodiscard]] float getAspectRatio() const;

    void setRoom(const std::shared_ptr<Room>& room);

    //region INTERNAL CALLS

    void internalForceSizeValues(int32_t width, int32_t height);

    void internalKeyEvent(int key, int scancode, int action, int mods);

    void internalCursorPosEvent(double x, double y);

    //endregion
};


#endif //RVTRACKING_APPLICATION_H
