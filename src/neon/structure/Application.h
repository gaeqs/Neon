//
// Created by grial on 19/10/22.
//

#ifndef RVTRACKING_APPLICATION_H
#define RVTRACKING_APPLICATION_H

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

namespace neon
{
    class Room;

    class Render;

    class CommandBuffer;

    class Application;

    /**
     * @brief An interface that abstracts the backend-specific details of an application.
     *
     * This class provides a common interface for different application backends,
     * such as Vulkan, QT, etc.
     *
     * It is responsible for windowing, input, and the main loop.
     */
    class ApplicationImplementation
    {
      public:
        ApplicationImplementation() = default;

        virtual ~ApplicationImplementation() = default;

        /**
         * @brief Initializes the application implementation.
         * @param application the application to link to.
         */
        virtual void init(Application* application) = 0;

        /**
         * @brief Returns the creation info of the application.
         * @return the creation info.
         */
        virtual const ApplicationCreateInfo& getCreationInfo() const = 0;

        /**
         * @brief Returns the command manager of the application.
         * @return the command manager.
         */
        [[nodiscard]] virtual const CommandManager& getCommandManager() const = 0;

        /**
         * @brief Returns the command manager of the application.
         * @return the command manager.
         */
        [[nodiscard]] virtual CommandManager& getCommandManager() = 0;

        /**
         * @brief Returns the size of the window.
         * @return the size of the window.
         */
        [[nodiscard]] virtual rush::Vec2i getWindowSize() const = 0;

        /**
         * @brief Returns the current frame information.
         * @return the current frame information.
         */
        [[nodiscard]] virtual FrameInformation getCurrentFrameInformation() const = 0;

        /**
         * @brief Returns the current command buffer.
         * @return the current command buffer.
         */
        [[nodiscard]] virtual CommandBuffer* getCurrentCommandBuffer() const = 0;

        /**
         * @brief Locks or unlocks the mouse.
         * @param lock whether to lock the mouse.
         */
        virtual void lockMouse(bool lock) = 0;

        /**
         * @brief Returns whether the application is in modal mode.
         * @return whether the application is in modal mode.
         */
        virtual bool isInModalMode() const = 0;

        /**
         * @brief Sets the modal mode of the application.
         * @param modal the modal mode.
         */
        virtual void setModalMode(bool modal) = 0;

        /**
         * @brief Starts the game loop.
         * This method will block until the application is closed.
         * @return a result indicating whether the loop finished successfully.
         */
        virtual Result<uint32_t, std::string> startGameLoop() = 0;

        /**
         * @brief Renders a frame.
         * @param room the room to render.
         */
        virtual void renderFrame(Room* room) = 0;

        /**
         * @brief Returns whether the current thread is the main thread.
         * @return whether the current thread is the main thread.
         */
        [[nodiscard]] virtual bool isMainThread() const = 0;
    };

    /**
     * @brief The main class of the application.
     *
     * This is the main, high-level, backend-agnostic representation of the application.
     * It owns all the core engine systems (Room, Render, AssetCollection, TaskRunner)
     * and uses an ApplicationImplementation to interact with the underlying platform.
     */
    class Application
    {
        std::unique_ptr<ApplicationImplementation> _implementation;

        std::shared_ptr<Room> _room;

        rush::Vec2d _lastCursorPosition;
        Profiler _profiler;
        AssetCollection _assets;
        AssetLoaderCollection _assetLoaders;
        TaskRunner _taskRunner;
        std::shared_ptr<Render> _render;
        std::optional<rush::Vec2i> _forcedViewport;

      public:
        /**
         * @brief Constructs a new Application.
         * @param implementation the application implementation to use.
         */
        explicit Application(std::unique_ptr<ApplicationImplementation> implementation);

        /**
         * @brief Destroys the application.
         */
        ~Application();

        /**
         * @brief Initializes the application.
         */
        void init();

        /**
         * @brief Starts the game loop.
         * This method will block until the application is closed.
         * @return a result indicating whether the loop finished successfully.
         */
        [[nodiscard]] Result<uint32_t, std::string> startGameLoop() const;

        /**
         * @brief Returns the application implementation.
         * @return the application implementation.
         */
        [[nodiscard]] const ApplicationImplementation* getImplementation() const;

        /**
         * @brief Returns the application implementation.
         * @return the application implementation.
         */
        [[nodiscard]] ApplicationImplementation* getImplementation();

        /**
         * @brief Returns the profiler.
         * @return the profiler.
         */
        [[nodiscard]] const Profiler& getProfiler() const;

        /**
         * @brief Returns the profiler.
         * @return the profiler.
         */
        [[nodiscard]] Profiler& getProfiler();

        /**
         * @brief Returns the command manager.
         * @return the command manager.
         */
        [[nodiscard]] const CommandManager& getCommandManager() const;

        /**
         * @brief Returns the command manager.
         * @return the command manager.
         */
        [[nodiscard]] CommandManager& getCommandManager();

        /**
         * @brief Returns the task runner.
         * @return the task runner.
         */
        [[nodiscard]] const TaskRunner& getTaskRunner() const;

        /**
         * @brief Returns the task runner.
         * @return the task runner.
         */
        [[nodiscard]] TaskRunner& getTaskRunner();

        /**
         * @brief Returns the asset collection.
         * @return the asset collection.
         */
        [[nodiscard]] const AssetCollection& getAssets() const;

        /**
         * @brief Returns the asset collection.
         * @return the asset collection.
         */
        [[nodiscard]] AssetCollection& getAssets();

        /**
         * @brief Returns the asset loader collection.
         * @return the asset loader collection.
         */
        [[nodiscard]] const AssetLoaderCollection& getAssetLoaders() const;

        /**
         * @brief Returns the asset loader collection.
         * @return the asset loader collection.
         */
        [[nodiscard]] AssetLoaderCollection& getAssetLoaders();

        /**
         * @brief Returns the width of the window.
         * @return the width of the window.
         */
        [[nodiscard]] int32_t getWidth() const;

        /**
         * @brief Returns the height of the window.
         * @return the height of the window.
         */
        [[nodiscard]] int32_t getHeight() const;

        /**
         * @brief Returns the aspect ratio of the window.
         * @return the aspect ratio of the window.
         */
        [[nodiscard]] float getAspectRatio() const;

        /**
         * @brief Returns the viewport of the application.
         * If a viewport is forced, it returns the forced viewport.
         * Otherwise, it returns the window size.
         * @return the viewport.
         */
        [[nodiscard]] rush::Vec2i getViewport() const;

        /**
         * @brief Forces the viewport to a specific size.
         * @param viewport the new viewport.
         */
        void forceViewport(rush::Vec2i viewport);

        /**
         * @brief Removes the forced viewport.
         */
        void removeForcedViewport();

        /**
         * @brief Returns the render system.
         * @return the render system.
         */
        [[nodiscard]] const std::shared_ptr<Render>& getRender() const;

        /**
         * @brief Sets the render system.
         * @param render the new render system.
         */
        void setRender(const std::shared_ptr<Render>& render);

        /**
         * @brief Returns the current frame information.
         * @return the current frame information.
         */
        [[nodiscard]] FrameInformation getCurrentFrameInformation() const;

        /**
         * @brief Returns the last cursor position.
         * @return the last cursor position.
         */
        [[nodiscard]] rush::Vec2d getLastCursorPosition() const;

        /**
         * @brief Returns the current command buffer.
         *
         * Only the main thread should use this command buffer!
         *
         * @return the current command buffer.
         */
        [[nodiscard]] CommandBuffer* getCurrentCommandBuffer() const;

        /**
         * @brief Returns the current room.
         * @return the current room.
         */
        [[nodiscard]] const std::shared_ptr<Room>& getRoom() const;

        /**
         * @brief Sets the current room.
         * @param room the new room.
         */
        void setRoom(const std::shared_ptr<Room>& room);

        /**
         * @brief Locks or unlocks the mouse.
         * @param lock whether to lock the mouse.
         */
        void lockMouse(bool lock);

        /**
         * @brief Returns whether the application is in modal mode.
         * @return whether the application is in modal mode.
         */
        bool isInModalMode() const;

        /**
         * @brief Sets the modal mode of the application.
         * @param modal the modal mode.
         */
        void setModalMode(bool modal);

        /**
         * Returns whether the thread calling this function is the main thread.
         */
        [[nodiscard]] bool isMainThread() const;

        // region INTERNAL CALLS

        /**
         * @brief Invokes a key event.
         * This method is called by the application implementation.
         * @param key the key code.
         * @param scancode the scancode.
         * @param action the action.
         * @param mods the modifiers.
         */
        void invokeKeyEvent(int key, int scancode, int action, int mods);

        /**
         * @brief Invokes a char event.
         * This method is called by the application implementation.
         * @param key the character.
         */
        void invokeCharEvent(char32_t key);

        /**
         * @brief Invokes a mouse button event.
         * This method is called by the application implementation.
         * @param button the button code.
         * @param action the action.
         * @param mods the modifiers.
         */
        void invokeMouseButtonEvent(int button, int action, int mods);

        /**
         * @brief Invokes a cursor position event.
         * This method is called by the application implementation.
         * @param x the x position.
         * @param y the y position.
         */
        void invokeCursorPosEvent(double x, double y);

        /**
         * @brief Invokes a scroll event.
         * This method is called by the application implementation.
         * @param xOffset the x offset.
         * @param yOffset the y offset.
         */
        void invokeScrollEvent(double xOffset, double yOffset);

        // endregion
    };
} // namespace neon

#endif // RVTRACKING_APPLICATION_H
