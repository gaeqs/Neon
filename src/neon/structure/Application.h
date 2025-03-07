//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_APPLICATION_H
#define RVTRACKING_APPLICATION_H


#include <cstdint>
#include <string>
#include <memory>
#include <optional>

#include <rush/rush.h>

#include <neon/structure/collection/AssetCollection.h>
#include <neon/loader/AssetLoaderCollection.h>
#include <neon/render/FrameInformation.h>
#include <neon/util/Result.h>
#include <neon/util/profile/Profiler.h>

#include <neon/render/buffer/CommandManager.h>
#include <neon/util/task/TaskRunner.h>

#include <neon/structure/ApplicationCreateInfo.h>


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

        virtual const ApplicationCreateInfo& getCreationInfo() const = 0;

        [[nodiscard]] virtual rush::Vec2i getWindowSize() const = 0;

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

        rush::Vec2d _lastCursorPosition;
        Profiler _profiler;
        AssetCollection _assets;
        AssetLoaderCollection _assetLoaders;
        TaskRunner _taskRunner;
        CommandManager _commandManager;
        std::shared_ptr<Render> _render;
        std::optional<rush::Vec2i> _forcedViewport;

    public:
        explicit Application(
            std::unique_ptr<ApplicationImplementation> implementation);

        ~Application();

        void init();

        [[nodiscard]] Result<uint32_t, std::string> startGameLoop();

        [[nodiscard]] const ApplicationImplementation*
        getImplementation() const;

        [[nodiscard]] ApplicationImplementation* getImplementation();

        [[nodiscard]] const Profiler& getProfiler() const;

        [[nodiscard]] Profiler& getProfiler();

        [[nodiscard]] const CommandManager& getCommandManager() const;

        [[nodiscard]] CommandManager& getCommandManager();

        [[nodiscard]] const TaskRunner& getTaskRunner() const;

        [[nodiscard]] TaskRunner& getTaskRunner();

        [[nodiscard]] const AssetCollection& getAssets() const;

        [[nodiscard]] AssetCollection& getAssets();

        [[nodiscard]] const AssetLoaderCollection& getAssetLoaders() const;

        [[nodiscard]] AssetLoaderCollection& getAssetLoaders();

        [[nodiscard]] int32_t getWidth() const;

        [[nodiscard]] int32_t getHeight() const;

        [[nodiscard]] float getAspectRatio() const;

        [[nodiscard]] rush::Vec2i getViewport() const;

        void forceViewport(rush::Vec2i viewport);

        void removeForcedViewport();

        [[nodiscard]] const std::shared_ptr<Render>& getRender() const;

        void setRender(const std::shared_ptr<Render>& render);

        [[nodiscard]] FrameInformation getCurrentFrameInformation() const;

        [[nodiscard]] rush::Vec2d getLastCursorPosition() const;

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
