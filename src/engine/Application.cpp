//
// Created by grial on 19/10/22.
//

#include "Application.h"

#include <engine/Room.h>
#include <engine/KeyboardEvent.h>
#include <engine/CursorEvent.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
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
        _width(width),
        _height(height),
        _window(nullptr),
        _room(nullptr),
        _last_cursor_pos(0.0, 0.0),
        _implementation() {
}

Application::~Application() {
    glfwTerminate();
}

Result<GLFWwindow*, std::string> Application::init(const std::string& name) {
    if (!glfwInit()) {
        return {"Failed to initialize GLFW"};
    }

    _implementation.preWindowCreation();

    _window = glfwCreateWindow(_width, _height, name.c_str(), nullptr, nullptr);
    if (!_window) {
        return {"Failed to open GLFW window"};
    }

    glfwSetWindowUserPointer(_window, this);
    glfwSetWindowSizeCallback(_window, framebuffer_size_callback);
    glfwSetKeyCallback(_window, key_size_callback);
    glfwSetCursorPosCallback(_window, cursor_pos_callback);

    _implementation.postWindowCreation(_window);

    return {_window};
}

Result<uint32_t, std::string> Application::startGameLoop() {
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

            float seconds = static_cast<float>(duration.count()) * 10.0e-9f;

            //std::cout << (1 / seconds) << std::endl;

            if (_implementation.preUpdate()) {
                glfwPollEvents();

                if (_room != nullptr) {
                    _room->update(seconds);
                    _room->preDraw();
                    _room->draw();
                }

                _implementation.endDraw();
            }

            ++frames;
        }
    } catch (const std::exception& exception) {
        return {exception.what()};
    }

    return {frames};
}

const Application::Implementation& Application::getImplementation() const {
    return _implementation;
}

Application::Implementation& Application::getImplementation() {
    return _implementation;
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
    if (room != nullptr && room->getApplication() != this) {
        throw std::runtime_error("Room's application is not this application!");
    }
    _room = room;
    _room->onResize();
}

void Application::lockMouse(bool lock) {
    if (lock) {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(_window, &_last_cursor_pos.x, &_last_cursor_pos.y);
    } else {
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
