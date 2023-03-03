//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_APPLICATION_H
#define RVTRACKING_APPLICATION_H


#include <cstdint>
#include <string>
#include <memory>

#include <glm/glm.hpp>

#include <engine/structure/collection/AssetCollection.h>
#include <engine/render/FrameInformation.h>
#include <util/Result.h>
#include <util/profile/Profiler.h>


#ifdef USE_VULKAN

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/VKApplication.h>

#endif

namespace neon {

    class Room;

    class Application {

    public:

#ifdef USE_VULKAN
        using Implementation = vulkan::VKApplication;
#endif

    protected:

        int32_t _width;
        int32_t _height;
        GLFWwindow* _window;

        std::shared_ptr<Room> _room;

        glm::dvec2 _lastCursorPosition;
        FrameInformation _currentFrameInformation;

        Implementation _implementation;
        Profiler _profiler;

        AssetCollection _assets;

    public:

        Application(int32_t width, int32_t height);

        ~Application();

        Result<GLFWwindow*, std::string> init(const std::string& name);

        [[nodiscard]] Result<uint32_t, std::string> startGameLoop();

        [[nodiscard]] const Implementation& getImplementation() const;

        [[nodiscard]] Implementation& getImplementation();

        [[nodiscard]]  const Profiler& getProfiler() const;

        [[nodiscard]] Profiler& getProfiler();

        [[nodiscard]] const AssetCollection& getAssets() const;

        [[nodiscard]] AssetCollection& getAssets();

        [[nodiscard]] int32_t getWidth() const;

        [[nodiscard]] int32_t getHeight() const;

        [[nodiscard]] float getAspectRatio() const;

        [[nodiscard]] FrameInformation getCurrentFrameInformation() const;

        void setRoom(const std::shared_ptr<Room>& room);

        void lockMouse(bool lock);

        //region INTERNAL CALLS

        void internalForceSizeValues(int32_t width, int32_t height);

        void internalKeyEvent(int key, int scancode, int action, int mods);

        void internalCursorPosEvent(double x, double y);

        //endregion
    };
}


#endif //RVTRACKING_APPLICATION_H
