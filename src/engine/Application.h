//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_APPLICATION_H
#define RVTRACKING_APPLICATION_H


#include <cstdint>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "../util/Result.h"

class Application {

    int32_t _width;
    int32_t _height;
    GLFWwindow* _window;

public:

    Application(int32_t width, int32_t height);

    Result<GLFWwindow*, std::string> init();

    Result<uint32_t, std::string> startGameLoop();

    int32_t getWidth();

    int32_t getHeight();

    float getAspectRatio();
};


#endif //RVTRACKING_APPLICATION_H
