//
// Created by grial on 19/10/22.
//

#include "Application.h"

#include <engine/Room.h>
#include <engine/KeyboardEvent.h>
#include <engine/CursorEvent.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    auto* application = static_cast<Application*>(glfwGetWindowUserPointer(
            window));
    application->internalForceSizeValues(width, height);
}

void key_size_callback(GLFWwindow* window, int key, int scancode, int action,
                       int mods) {
    auto* application = static_cast<Application*>(glfwGetWindowUserPointer(
            window));

    application->internalKeyEvent(key, scancode, action, mods);
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
    auto* application = static_cast<Application*>(glfwGetWindowUserPointer(
            window));
    application->internalCursorPosEvent(x, y);
}

Application::Application(int32_t width, int32_t height) :
        _width(width), _height(height), _window(nullptr),
        _last_cursor_pos(0.0, 0.0) {
}

Result<GLFWwindow*, std::string> Application::init(const std::string& name) {
    if (!glfwInit()) {
        return {"Failed to initialize GLFW"};
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    _window = glfwCreateWindow(_width, _height, name.c_str(), nullptr,
                               nullptr);
    if (!_window) {
        return {"Failed to open GLFW window"};
    }

    // Enable vertical sync (on cards that support it)
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return {"Failed to initialize GLAD"};
    }


    glfwSetWindowUserPointer(_window, this);
    glfwSetWindowSizeCallback(_window, framebuffer_size_callback);
    glfwSetKeyCallback(_window, key_size_callback);
    glfwSetCursorPosCallback(_window, cursor_pos_callback);

    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(_window, &_last_cursor_pos.x, &_last_cursor_pos.y);

    return {_window};
}

Result<uint32_t, std::string> Application::startGameLoop() const {
    if (_window == nullptr) {
        return {"Window is not initialized"};
    }

    uint32_t frames = 0;

    auto last_tick = std::chrono::high_resolution_clock::now();
    try {
        while (!glfwWindowShouldClose(_window)) {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = now - last_tick;
            last_tick = now;

            float seconds = static_cast<float>(duration.count())
                            / 1000000000.0f;

            //std::cout << (1 / seconds) << std::endl;

            if (_room != nullptr) {
                _room->update(seconds);
            }

            glfwPollEvents();

            if (_room != nullptr) {
                _room->draw();
            }

            glfwSwapBuffers(_window);
            frames++;
        }
    } catch (const std::exception& exception) {
        glfwTerminate();
        return {exception.what()};
    }

    glfwTerminate();

    return {frames};
}

int32_t Application::getWidth() const {
    return _width;
}

int32_t Application::getHeight() const {
    return _height;
}

float Application::getAspectRatio() const {
    return static_cast<float>(_width) / static_cast<float>(_height);
}

void Application::setRoom(const std::shared_ptr<Room>& room) {
    if (_room != nullptr) {
        _room->_application = nullptr;
    }
    _room = room;
    if (_room != nullptr) {
        _room->_application = this;
        _room->onResize();
    }
}

void Application::internalForceSizeValues(int32_t width, int32_t height) {
    _width = width;
    _height = height;
    if (_room != nullptr) {
        _room->onResize();
    }
}

void Application::internalKeyEvent(int key, int scancode,
                                   int action, int mods) {
    KeyboardEvent event{
            mods,
            scancode,
            static_cast<KeyboardKey>(key),
            static_cast<KeyboardAction>(action)
    };
    if (_room != nullptr) {
        _room->onKey(event);
    }
}

void Application::internalCursorPosEvent(double x, double y) {
    glm::dvec2 current(x, y);
    auto delta = current - _last_cursor_pos;
    _last_cursor_pos = current;

    CursorMoveEvent event{
            current,
            delta
    };

    if (_room != nullptr) {
        _room->onCursorMove(event);
    }
}