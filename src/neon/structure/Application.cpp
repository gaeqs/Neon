//
// Created by grial on 19/10/22.
//

#include "Application.h"

#include <neon/structure/Room.h>
#include <neon/io/KeyboardEvent.h>
#include <neon/io/CursorEvent.h>
#include <neon/io/MouseButtonEvent.h>
#include <neon/io/ScrollEvent.h>

namespace neon
{
    Application::Application(std::unique_ptr<ApplicationImplementation> implementation) :
        _implementation(std::move(implementation)),
        _room(nullptr),
        _assetLoaders(true),
        _lastCursorPosition(0.0, 0.0),
        _commandManager(this)
    {
    }

    Application::~Application() = default;

    void Application::init()
    {
        _implementation->init(this);
    }

    Result<uint32_t, std::string> Application::startGameLoop()
    {
        return _implementation->startGameLoop();
    }

    const ApplicationImplementation* Application::getImplementation() const
    {
        return _implementation.get();
    }

    ApplicationImplementation* Application::getImplementation()
    {
        return _implementation.get();
    }

    const Profiler& Application::getProfiler() const
    {
        return _profiler;
    }

    Profiler& Application::getProfiler()
    {
        return _profiler;
    }

    const CommandManager& Application::getCommandManager() const
    {
        return _commandManager;
    }

    CommandManager& Application::getCommandManager()
    {
        return _commandManager;
    }

    const TaskRunner& Application::getTaskRunner() const
    {
        return _taskRunner;
    }

    TaskRunner& Application::getTaskRunner()
    {
        return _taskRunner;
    }

    const AssetCollection& Application::getAssets() const
    {
        return _assets;
    }

    AssetCollection& Application::getAssets()
    {
        return _assets;
    }

    const AssetLoaderCollection& Application::getAssetLoaders() const
    {
        return _assetLoaders;
    }

    AssetLoaderCollection& Application::getAssetLoaders()
    {
        return _assetLoaders;
    }

    int32_t Application::getWidth() const
    {
        return _implementation->getWindowSize().x();
    }

    int32_t Application::getHeight() const
    {
        return _implementation->getWindowSize().y();
    }

    float Application::getAspectRatio() const
    {
        auto vp = _implementation->getWindowSize();
        return static_cast<float>(vp.x()) / static_cast<float>(vp.y());
    }

    rush::Vec2i Application::getViewport() const
    {
        if (_forcedViewport.has_value()) {
            return _forcedViewport.value();
        }
        return _implementation->getWindowSize();
    }

    void Application::forceViewport(rush::Vec2i viewport)
    {
        _forcedViewport = viewport;
    }

    void Application::removeForcedViewport()
    {
        _forcedViewport = {};
    }

    const std::shared_ptr<Render>& Application::getRender() const
    {
        return _render;
    }

    void Application::setRender(const std::shared_ptr<Render>& render)
    {
        _render = render;
    }

    FrameInformation Application::getCurrentFrameInformation() const
    {
        return _implementation->getCurrentFrameInformation();
    }

    rush::Vec2d Application::getLastCursorPosition() const
    {
        return _lastCursorPosition;
    }

    CommandBuffer* Application::getCurrentCommandBuffer() const
    {
        return _implementation->getCurrentCommandBuffer();
    }

    const std::shared_ptr<Room>& Application::getRoom() const
    {
        return _room;
    }

    void Application::setRoom(const std::shared_ptr<Room>& room)
    {
        if (room != nullptr && room->getApplication() != this) {
            throw std::runtime_error("Room's application is not this application!");
        }
        _room = room;
    }

    void Application::lockMouse(bool lock)
    {
        _implementation->lockMouse(lock);
    }
    bool Application::isMainThread() const
    {
        return _implementation->isMainThread();
    }

    void Application::invokeKeyEvent(int key, int scancode, int action, int mods)
    {
        KeyboardEvent event{scancode, mods, static_cast<KeyboardKey>(key), static_cast<KeyboardAction>(action)};
        if (_room != nullptr) {
            _room->onKey(event);
        }
    }

    void Application::invokeMouseButtonEvent(int button, int action, int mods)
    {
        MouseButtonEvent event{static_cast<MouseButton>(button), static_cast<KeyboardAction>(action), mods};
        if (_room != nullptr) {
            _room->onMouseButton(event);
        }
    }

    void Application::invokeCursorPosEvent(double x, double y)
    {
        rush::Vec2d current(x, y);
        auto delta = current - _lastCursorPosition;
        _lastCursorPosition = current;

        CursorMoveEvent event{current.cast<float>(), delta.cast<float>()};

        if (_room != nullptr) {
            _room->onCursorMove(event);
        }
    }

    void Application::invokeScrollEvent(double xOffset, double yOffset)
    {
        ScrollEvent event{
            {xOffset, yOffset}
        };
        if (_room != nullptr) {
            _room->onScroll(event);
        }
    }
} // namespace neon
