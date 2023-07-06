//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_APPLICATION_H
#define RVTRACKING_APPLICATION_H


#include <cstdint>
#include <string>
#include <memory>
#include <optional>

#include <glm/glm.hpp>

#include <engine/structure/collection/AssetCollection.h>
#include <engine/render/FrameInformation.h>
#include <util/Result.h>
#include <util/profile/Profiler.h>

namespace neon {

    class Room;

    class Render;

    class CommandBuffer;

    class Application;

    class ApplicationImplementation {

    public:

        ApplicationImplementation() = default;

        virtual ~ApplicationImplementation() = default;

        virtual void init(Application* application) = 0;

        [[nodiscard]] virtual glm::ivec2 getWindowSize() const = 0;

        [[nodiscard]] virtual FrameInformation
        getCurrentFrameInformation() const = 0;

        [[nodiscard]] virtual CommandBuffer*
        getCurrentCommandBuffer() const = 0;

        virtual void lockMouse(bool lock) = 0;

        virtual Result<uint32_t, std::string> startGameLoop() = 0;

        virtual void renderFrame(Room* room) = 0;

    };

    class Application {

        std::unique_ptr<ApplicationImplementation> _implementation;

        std::shared_ptr<Room> _room;

        glm::dvec2 _lastCursorPosition;
        Profiler _profiler;
        AssetCollection _assets;
        std::shared_ptr<Render> _render;
        std::optional<glm::ivec2> _forcedViewport;

    public:

        explicit Application(
                std::unique_ptr<ApplicationImplementation> implementation);

        void init();

        [[nodiscard]] Result<uint32_t, std::string> startGameLoop();

        [[nodiscard]] const ApplicationImplementation*
        getImplementation() const;

        [[nodiscard]] ApplicationImplementation* getImplementation();

        [[nodiscard]] const Profiler& getProfiler() const;

        [[nodiscard]] Profiler& getProfiler();

        [[nodiscard]] const AssetCollection& getAssets() const;

        [[nodiscard]] AssetCollection& getAssets();

        [[nodiscard]] int32_t getWidth() const;

        [[nodiscard]] int32_t getHeight() const;

        [[nodiscard]] float getAspectRatio() const;

        [[nodiscard]] glm::ivec2 getViewport() const;

        void forceViewport(glm::ivec2 viewport);

        void removeForcedViewport();

        [[nodiscard]] const std::shared_ptr<Render>& getRender() const;

        void setRender(const std::shared_ptr<Render>& render);

        [[nodiscard]] FrameInformation getCurrentFrameInformation() const;

        [[nodiscard]] glm::dvec2 getLastCursorPosition() const;

        [[nodiscard]] CommandBuffer* getCurrentCommandBuffer() const;

        [[nodiscard]] const std::shared_ptr<Room>& getRoom() const;

        void setRoom(const std::shared_ptr<Room>& room);

        void lockMouse(bool lock);

        //region INTERNAL CALLS

        void invokeKeyEvent(int key, int scancode, int action, int mods);

        void invokeMouseButtonEvent(int button, int action, int mods);

        void invokeCursorPosEvent(double x, double y);

        void invokeScrollEvent(double xOffset, double yOffset);

        //endregion
    };
}


#endif //RVTRACKING_APPLICATION_H
