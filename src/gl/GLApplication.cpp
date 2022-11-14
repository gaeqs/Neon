//
// Created by gaelr on 10/11/2022.
//

#include "GLApplication.h"

#include <stdexcept>

void GLApplication::preWindowCreation() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void GLApplication::postWindowCreation(GLFWwindow* window) {
    _window = window;

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
}

void GLApplication::preUpdate() {

}

void GLApplication::preDraw() {

}

void GLApplication::postDraw() {
    glfwSwapBuffers(_window);
}

void GLApplication::internalForceSizeValues(int32_t width, int32_t height) {
    glViewport(0, 0, width, height);
}

void
GLApplication::internalKeyEvent(int key, int scancode, int action, int mods) {

}

void GLApplication::internalCursorPosEvent(double x, double y) {

}
