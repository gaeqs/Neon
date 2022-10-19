//
// Created by grial on 19/10/22.
//

#include <iostream>
#include "Application.h"

Application::Application(int32_t width, int32_t height) :
        _width(width), _height(height), _window(nullptr) {
}

Result<GLFWwindow*, std::string> Application::init() {
    if (!glfwInit()) {
        return {"Failed to initialize GLFW"};
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    _window = glfwCreateWindow(_width, _height, "Spinning Triangle", nullptr, nullptr);
    if (!_window) {
        return {"Failed to open GLFW window"};
    }

    // Enable vertical sync (on cards that support it)
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        return {"Failed to initialize GLAD"};
    }

    return {_window};
}

Result<uint32_t, std::string> Application::startGameLoop() {
    if (_window == nullptr) {
        return {"Window is not initialized"};
    }

    uint32_t frames = 0;

    try {
        while (!glfwWindowShouldClose(_window)) {
            frames++;

            glfwSwapBuffers(_window);
        }
    } catch (const std::exception& exception) {
        glfwTerminate();
        return {exception.what()};
    }

    glfwTerminate();

    return {frames};
}

int32_t Application::getWidth() {
    return _width;
}

int32_t Application::getHeight() {
    return _height;
}

float Application::getAspectRatio() {
    return static_cast<float>(_width) / static_cast<float>(_height);
}
