//
// Created by gaelr on 10/11/2022.
//

#ifndef NEON_GLAPPLICATION_H
#define NEON_GLAPPLICATION_H

#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLApplication {

    GLFWwindow* _window;

public:

    void preWindowCreation();

    void postWindowCreation(GLFWwindow* window);

    void preUpdate();

    void preDraw();

    void postDraw();

    void internalForceSizeValues(int32_t width, int32_t height);

    void internalKeyEvent(int key, int scancode, int action, int mods);

    void internalCursorPosEvent(double x, double y);

};


#endif //NEON_GLAPPLICATION_H
