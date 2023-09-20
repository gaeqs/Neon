//
// Created by grial on 19/10/22.
//

#include "Application.h"

#include <engine/structure/Room.h>
#include <engine/io/KeyboardEvent.h>
#include <engine/io/CursorEvent.h>
#include <engine/io/MouseButtonEvent.h>
#include <engine/io/ScrollEvent.h>

namespace neon {

    Application::Application(
            std::unique_ptr<ApplicationImplementation> implementation) :
            _implementation(std::move(implementation)),
            _room(nullptr),
            _lastCursorPosition(0.0, 0.0),
            _profiler(),
            _assets(),
            _render(),
            _forcedViewport() {
    }

    void Application::init() {
        _implementation->init(this);
    }

    Result<uint32_t, std::string> Application::startGameLoop() {
        return _implementation->startGameLoop();
    }

    const ApplicationImplementation* Application::getImplementation() const {
        return _implementation.get();
    }

    ApplicationImplementation* Application::getImplementation() {
        return _implementation.get();
    }

    const Profiler& Application::getProfiler() const {
        return _profiler;
    }

    Profiler& Application::getProfiler() {
        return _profiler;
    }

    const AssetCollection& Application::getAssets() const {
        return _assets;
    }

    AssetCollection& Application::getAssets() {
        return _assets;
    }

    int32_t Application::getWidth() const {
        return _implementation->getWindowSize().x;
    }

    int32_t Application::getHeight() const {
        return _implementation->getWindowSize().y;
    }

    float Application::getAspectRatio() const {
        auto vp = _implementation->getWindowSize();
        return static_cast<float>(vp.x) / static_cast<float>(vp.y);
    }

    glm::ivec2 Application::getViewport() const {
        if (_forcedViewport.has_value()) {
            return _forcedViewport.value();
        }
        return _implementation->getWindowSize();
    }

    void Application::forceViewport(glm::ivec2 viewport) {
        _forcedViewport = viewport;
    }

    void Application::removeForcedViewport() {
        _forcedViewport = {};
    }

    const std::shared_ptr<Render>& Application::getRender() const {
        return _render;
    }

    void Application::setRender(const std::shared_ptr<Render>& render) {
        _render = render;
    }

    FrameInformation Application::getCurrentFrameInformation() const {
        return _implementation->getCurrentFrameInformation();
    }

    glm::dvec2 Application::getLastCursorPosition() const {
        return _lastCursorPosition;
    }

    CommandBuffer* Application::getCurrentCommandBuffer() const {
        return _implementation->getCurrentCommandBuffer();
    }

    const std::shared_ptr<Room>& Application::getRoom() const {
        return _room;
    }

    void Application::setRoom(const std::shared_ptr<Room>& room) {
        if (room != nullptr && room->getApplication() != this) {
            throw std::runtime_error(
                    "Room's application is not this application!");
        }
        _room = room;
    }

    void Application::lockMouse(bool lock) {
        _implementation->lockMouse(lock);
    }

    void Application::invokeKeyEvent(int key, int scancode,
                                     int action, int mods) {
        KeyboardEvent event{
                scancode,
                mods,
                static_cast<KeyboardKey>(key),
                static_cast<KeyboardAction>(action)
        };
        if (_room != nullptr) {
            _room->onKey(event);
        }
    }

    void Application::invokeMouseButtonEvent(int button, int action, int mods) {
        MouseButtonEvent event{
                static_cast<MouseButton>(button),
                static_cast<KeyboardAction>(action),
                mods
        };
        if (_room != nullptr) {
            _room->onMouseButton(event);
        }
    }

    void Application::invokeCursorPosEvent(double x, double y) {
        glm::dvec2 current(x, y);
        auto delta = current - _lastCursorPosition;
        _lastCursorPosition = current;

        CursorMoveEvent event{
                current,
                delta
        };

        if (_room != nullptr) {
            _room->onCursorMove(event);
        }
    }

    void Application::invokeScrollEvent(double xOffset, double yOffset) {
        ScrollEvent event{{xOffset, yOffset}};
        if (_room != nullptr) {
            _room->onScroll(event);
        }
    }
}