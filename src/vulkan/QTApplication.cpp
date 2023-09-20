/*
 *  Copyright (c) 2015-2023 VG-Lab/URJC.
 *
 *  Authors: Gael Rial Costas  <gael.rial.costas@urjc.es>
 *
 *  This file is part of ViSimpl <https://github.com/vg-lab/visimpl>
 *
 *  This library is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU Lesser General Public License version 3.0 as published
 *  by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//
// Created by grial on 7/06/23.
//

#include "QTApplication.h"

#include <iostream>
#include <utility>

#include <engine/structure/Room.h>

#include <QMouseEvent>
#include <QWheelEvent>
#include <engine/io/KeyboardEvent.h>

namespace {
    bool debugFilter(VkDebugReportFlagsEXT flags,
                     VkDebugReportObjectTypeEXT objectType, uint64_t object,
                     size_t location, int32_t messageCode,
                     const char* pLayerPrefix, const char* pMessage) {
        std::cerr << pMessage << std::endl;
        return false;
    }
}

neon::vulkan::QTApplication::QTApplication() :
        _application(nullptr),
        _onInit(),
        _currentFrameInformation(0, 0.016f, 0.0f),
        _lastFrameTime(std::chrono::high_resolution_clock::now()),
        _lastFrameProcessTime(0.0f),
        _currentCommandBuffer(),
        _swapChainCount(0),
        _recording(false) {
}

void neon::vulkan::QTApplication::init(neon::Application* application) {
    _application = application;
}

glm::ivec2 neon::vulkan::QTApplication::getWindowSize() const {
    QSize s = size();
    return {s.width(), s.height()};
}

neon::FrameInformation
neon::vulkan::QTApplication::getCurrentFrameInformation() const {
    return _currentFrameInformation;
}

neon::CommandBuffer*
neon::vulkan::QTApplication::getCurrentCommandBuffer() const {
    if (!isRecordingCommandBuffer()) {
        return nullptr;
    }
    if (_currentCommandBuffer == nullptr) {
        _currentCommandBuffer = std::make_unique<CommandBuffer>(
                _application,
                currentCommandBuffer()
        );
    }
    return _currentCommandBuffer.get();
}

void neon::vulkan::QTApplication::lockMouse(bool lock) {
    // TODO
}

neon::Result<uint32_t, std::string>
neon::vulkan::QTApplication::startGameLoop() {
    return {"QTApplicaiton doesn't contain a GameLoop, as Qt already has one."};
}

void neon::vulkan::QTApplication::renderFrame(neon::Room* room) {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now - _lastFrameTime;
    _lastFrameTime = now;

    float seconds = static_cast<float>(duration.count()) * 1e-9f;

    _currentFrameInformation = {
            _currentFrameInformation.currentFrame + 1,
            seconds,
            _lastFrameProcessTime
    };

    _application->getRender()->checkFrameBufferRecreationConditions();

    if (room != nullptr) {
        room->update(_currentFrameInformation.currentDeltaTime);
        room->preDraw();
        room->draw();
        frameReady();
        requestUpdate();
    }

    now = std::chrono::high_resolution_clock::now();
    auto processTime = now - _lastFrameTime;
    _lastFrameProcessTime = static_cast<float>(processTime.count()) * 1e-9f;
    _currentCommandBuffer = nullptr;
}

VkInstance neon::vulkan::QTApplication::getInstance() const {
    return vulkanInstance()->vkInstance();
}

VkDevice neon::vulkan::QTApplication::getDevice() const {
    return device();
}

VkPhysicalDevice neon::vulkan::QTApplication::getPhysicalDevice() const {
    return physicalDevice();
}

VkQueue neon::vulkan::QTApplication::getGraphicsQueue() const {
    return graphicsQueue();
}

VkFormat neon::vulkan::QTApplication::getSwapChainImageFormat() const {
    return colorFormat();
}

VkFormat neon::vulkan::QTApplication::getDepthImageFormat() const {
    return depthStencilFormat();
}

VkCommandPool neon::vulkan::QTApplication::getCommandPool() const {
    return graphicsCommandPool();
}

VkSwapchainKHR neon::vulkan::QTApplication::getSwapChain() const {
    std::cerr << "QTApplication doesn't expose a swap chain!" << std::endl;
    return nullptr;
}

uint32_t neon::vulkan::QTApplication::getMaxFramesInFlight() const {
    return MAX_CONCURRENT_FRAME_COUNT;
}

uint32_t neon::vulkan::QTApplication::getCurrentFrame() const {
    return concurrentFrameCount();
}

uint32_t neon::vulkan::QTApplication::getCurrentSwapChainImage() const {
    return swapChainImageCount();
}

uint32_t neon::vulkan::QTApplication::getSwapChainCount() const {
    return _swapChainCount;
}

VkExtent2D neon::vulkan::QTApplication::getSwapChainExtent() const {
    QSize s = swapChainImageSize();
    return {static_cast<uint32_t>(s.width()),
            static_cast<uint32_t>(s.height())};
}

VkDescriptorPool neon::vulkan::QTApplication::getImGuiPool() const {
    std::cerr << "QTApplication doesn't support ImGUI" << std::endl;
    return nullptr;
}

bool neon::vulkan::QTApplication::isRecordingCommandBuffer() const {
    return _recording;
}

void neon::vulkan::QTApplication::preInitResources() {
    vulkanInstance()->installDebugOutputFilter(debugFilter);
}

void neon::vulkan::QTApplication::initResources() {
    if (_onInit) {
        _onInit(this);
    }
}

void neon::vulkan::QTApplication::initSwapChainResources() {
    ++_swapChainCount;
}

void neon::vulkan::QTApplication::releaseSwapChainResources() {
}

void neon::vulkan::QTApplication::releaseResources() {
}

void neon::vulkan::QTApplication::startNextFrame() {
    _recording = true;
    if (_application != nullptr) {
        renderFrame(_application->getRoom().get());
    }
    _recording = false;
}

void neon::vulkan::QTApplication::physicalDeviceLost() {
}

void neon::vulkan::QTApplication::logicalDeviceLost() {
}

QVulkanWindowRenderer* neon::vulkan::QTApplication::createRenderer() {
    return this;
}

void neon::vulkan::QTApplication::setInitializationFunction(
        std::function<void(QTApplication * )> func) {
    _onInit = std::move(func);
}

void neon::vulkan::QTApplication::mouseMoveEvent(QMouseEvent* event) {
    _application->invokeCursorPosEvent(event->x(), event->y());
}

void neon::vulkan::QTApplication::mousePressEvent(QMouseEvent* event) {
    auto qtMod = event->modifiers();
    int modifier = 0;
    if (qtMod & Qt::ShiftModifier)
        modifier |= (int) neon::KeyboardModifier::SHIFT;
    if (qtMod & Qt::ControlModifier)
        modifier |= (int) neon::KeyboardModifier::CONTROL;
    if (qtMod & Qt::AltModifier)
        modifier |= (int) neon::KeyboardModifier::ALT;
    if (qtMod & Qt::MetaModifier)
        modifier |= (int) neon::KeyboardModifier::SUPER;


    _application->invokeMouseButtonEvent(
            qtToGLFWMouseButton(event->button()),
            GLFW_PRESS,
            modifier
    );
}

void neon::vulkan::QTApplication::mouseReleaseEvent(QMouseEvent* event) {
    auto qtMod = event->modifiers();
    int modifier = 0;
    if (qtMod & Qt::ShiftModifier)
        modifier |= (int) neon::KeyboardModifier::SHIFT;
    if (qtMod & Qt::ControlModifier)
        modifier |= (int) neon::KeyboardModifier::CONTROL;
    if (qtMod & Qt::AltModifier)
        modifier |= (int) neon::KeyboardModifier::ALT;
    if (qtMod & Qt::MetaModifier)
        modifier |= (int) neon::KeyboardModifier::SUPER;


    _application->invokeMouseButtonEvent(
            qtToGLFWMouseButton(event->button()),
            GLFW_RELEASE,
            modifier
    );
}

void neon::vulkan::QTApplication::keyPressEvent(QKeyEvent* event) {
    auto qtMod = event->modifiers();
    int modifier = 0;
    if (qtMod & Qt::ShiftModifier)
        modifier |= (int) neon::KeyboardModifier::SHIFT;
    if (qtMod & Qt::ControlModifier)
        modifier |= (int) neon::KeyboardModifier::CONTROL;
    if (qtMod & Qt::AltModifier)
        modifier |= (int) neon::KeyboardModifier::ALT;
    if (qtMod & Qt::MetaModifier)
        modifier |= (int) neon::KeyboardModifier::SUPER;

    _application->invokeKeyEvent(
            qtToGLFWKey((Qt::Key) event->key()),
            event->nativeScanCode(),
            event->isAutoRepeat() ? 2 : 1,
            modifier
    );
}

void neon::vulkan::QTApplication::keyReleaseEvent(QKeyEvent* event) {
    auto qtMod = event->modifiers();
    int modifier = 0;
    if (qtMod & Qt::ShiftModifier)
        modifier |= (int) neon::KeyboardModifier::SHIFT;
    if (qtMod & Qt::ControlModifier)
        modifier |= (int) neon::KeyboardModifier::CONTROL;
    if (qtMod & Qt::AltModifier)
        modifier |= (int) neon::KeyboardModifier::ALT;
    if (qtMod & Qt::MetaModifier)
        modifier |= (int) neon::KeyboardModifier::SUPER;

    _application->invokeKeyEvent(
            qtToGLFWKey((Qt::Key) event->key()),
            event->nativeScanCode(),
            0,
            modifier
    );
}

void neon::vulkan::QTApplication::wheelEvent(QWheelEvent* event) {
    auto delta = event->angleDelta() / 15.0f;
    _application->invokeScrollEvent(delta.x(), delta.y());
}

int32_t neon::vulkan::QTApplication::qtToGLFWKey(Qt::Key key) {
    switch (key) {
        case Qt::Key_Space:
            return GLFW_KEY_SPACE;
        case Qt::Key_Apostrophe:
            return GLFW_KEY_APOSTROPHE;
        case Qt::Key_Comma:
            return GLFW_KEY_COMMA;
        case Qt::Key_Minus:
            return GLFW_KEY_MINUS;
        case Qt::Key_Period:
            return GLFW_KEY_PERIOD;
        case Qt::Key_Slash:
            return GLFW_KEY_SLASH;
        case Qt::Key_0:
            return GLFW_KEY_0;
        case Qt::Key_1:
            return GLFW_KEY_1;
        case Qt::Key_2:
            return GLFW_KEY_2;
        case Qt::Key_3:
            return GLFW_KEY_3;
        case Qt::Key_4:
            return GLFW_KEY_4;
        case Qt::Key_5:
            return GLFW_KEY_5;
        case Qt::Key_6:
            return GLFW_KEY_6;
        case Qt::Key_7:
            return GLFW_KEY_7;
        case Qt::Key_8:
            return GLFW_KEY_8;
        case Qt::Key_9:
            return GLFW_KEY_9;
        case Qt::Key_Semicolon:
            return GLFW_KEY_SEMICOLON;
        case Qt::Key_Equal:
            return GLFW_KEY_EQUAL;
        case Qt::Key_A:
            return GLFW_KEY_A;
        case Qt::Key_B:
            return GLFW_KEY_B;
        case Qt::Key_C:
            return GLFW_KEY_C;
        case Qt::Key_D:
            return GLFW_KEY_D;
        case Qt::Key_E:
            return GLFW_KEY_E;
        case Qt::Key_F:
            return GLFW_KEY_F;
        case Qt::Key_G:
            return GLFW_KEY_G;
        case Qt::Key_H:
            return GLFW_KEY_H;
        case Qt::Key_I:
            return GLFW_KEY_I;
        case Qt::Key_J:
            return GLFW_KEY_J;
        case Qt::Key_K:
            return GLFW_KEY_K;
        case Qt::Key_L:
            return GLFW_KEY_L;
        case Qt::Key_M:
            return GLFW_KEY_M;
        case Qt::Key_N:
            return GLFW_KEY_N;
        case Qt::Key_O:
            return GLFW_KEY_O;
        case Qt::Key_P:
            return GLFW_KEY_P;
        case Qt::Key_Q:
            return GLFW_KEY_Q;
        case Qt::Key_R:
            return GLFW_KEY_R;
        case Qt::Key_S:
            return GLFW_KEY_S;
        case Qt::Key_T:
            return GLFW_KEY_T;
        case Qt::Key_U:
            return GLFW_KEY_U;
        case Qt::Key_V:
            return GLFW_KEY_V;
        case Qt::Key_W:
            return GLFW_KEY_W;
        case Qt::Key_X:
            return GLFW_KEY_X;
        case Qt::Key_Y:
            return GLFW_KEY_Y;
        case Qt::Key_Z:
            return GLFW_KEY_Z;
        case Qt::Key_BracketLeft:
            return GLFW_KEY_LEFT_BRACKET;
        case Qt::Key_Backslash:
            return GLFW_KEY_BACKSLASH;
        case Qt::Key_BracketRight:
            return GLFW_KEY_RIGHT_BRACKET;
        case Qt::Key_Agrave:
            return GLFW_KEY_GRAVE_ACCENT;
        case Qt::Key_Escape:
            return GLFW_KEY_ESCAPE;
        case Qt::Key_Return:
            return GLFW_KEY_ENTER;
        case Qt::Key_Enter:
            return GLFW_KEY_KP_ENTER;
        case Qt::Key_Tab:
            return GLFW_KEY_TAB;
        case Qt::Key_Backspace:
            return GLFW_KEY_BACKSPACE;
        case Qt::Key_Insert:
            return GLFW_KEY_INSERT;
        case Qt::Key_Delete:
            return GLFW_KEY_DELETE;
        case Qt::Key_Right:
            return GLFW_KEY_RIGHT;
        case Qt::Key_Left:
            return GLFW_KEY_LEFT;
        case Qt::Key_Down:
            return GLFW_KEY_DOWN;
        case Qt::Key_Up:
            return GLFW_KEY_UP;
        case Qt::Key_PageUp:
            return GLFW_KEY_PAGE_UP;
        case Qt::Key_PageDown:
            return GLFW_KEY_PAGE_DOWN;
        case Qt::Key_Home:
            return GLFW_KEY_HOME;
        case Qt::Key_End:
            return GLFW_KEY_END;
        case Qt::Key_CapsLock:
            return GLFW_KEY_CAPS_LOCK;
        case Qt::Key_ScrollLock:
            return GLFW_KEY_SCROLL_LOCK;
        case Qt::Key_NumLock:
            return GLFW_KEY_NUM_LOCK;
        case Qt::Key_Print:
            return GLFW_KEY_PRINT_SCREEN;
        case Qt::Key_Pause:
            return GLFW_KEY_PAUSE;
        case Qt::Key_F1:
            return GLFW_KEY_F1;
        case Qt::Key_F2:
            return GLFW_KEY_F2;
        case Qt::Key_F3:
            return GLFW_KEY_F3;
        case Qt::Key_F4:
            return GLFW_KEY_F4;
        case Qt::Key_F5:
            return GLFW_KEY_F5;
        case Qt::Key_F6:
            return GLFW_KEY_F6;
        case Qt::Key_F7:
            return GLFW_KEY_F7;
        case Qt::Key_F8:
            return GLFW_KEY_F8;
        case Qt::Key_F9:
            return GLFW_KEY_F9;
        case Qt::Key_F10:
            return GLFW_KEY_F10;
        case Qt::Key_F11:
            return GLFW_KEY_F11;
        case Qt::Key_F12:
            return GLFW_KEY_F12;
        case Qt::Key_F13:
            return GLFW_KEY_F13;
        case Qt::Key_F14:
            return GLFW_KEY_F14;
        case Qt::Key_F15:
            return GLFW_KEY_F15;
        case Qt::Key_F16:
            return GLFW_KEY_F16;
        case Qt::Key_F17:
            return GLFW_KEY_F17;
        case Qt::Key_F18:
            return GLFW_KEY_F18;
        case Qt::Key_F19:
            return GLFW_KEY_F19;
        case Qt::Key_F20:
            return GLFW_KEY_F20;
        case Qt::Key_F21:
            return GLFW_KEY_F21;
        case Qt::Key_F22:
            return GLFW_KEY_F22;
        case Qt::Key_F23:
            return GLFW_KEY_F23;
        case Qt::Key_F24:
            return GLFW_KEY_F24;
        case Qt::Key_F25:
            return GLFW_KEY_F25;
        case Qt::Key_Launch0:
            return GLFW_KEY_KP_0;
        case Qt::Key_Launch1:
            return GLFW_KEY_KP_1;
        case Qt::Key_Launch2:
            return GLFW_KEY_KP_2;
        case Qt::Key_Launch3:
            return GLFW_KEY_KP_3;
        case Qt::Key_Launch4:
            return GLFW_KEY_KP_4;
        case Qt::Key_Launch5:
            return GLFW_KEY_KP_5;
        case Qt::Key_Launch6:
            return GLFW_KEY_KP_6;
        case Qt::Key_Launch7:
            return GLFW_KEY_KP_7;
        case Qt::Key_Launch8:
            return GLFW_KEY_KP_8;
        case Qt::Key_Launch9:
            return GLFW_KEY_KP_9;
        case Qt::Key_Asterisk:
            return GLFW_KEY_KP_MULTIPLY;
        case Qt::Key_Plus:
            return GLFW_KEY_KP_ADD;
        case Qt::Key_Shift:
            return GLFW_KEY_LEFT_SHIFT;
        case Qt::Key_Control:
            return GLFW_KEY_LEFT_CONTROL;
        case Qt::Key_Alt:
            return GLFW_KEY_LEFT_ALT;
        case Qt::Key_AltGr:
            return GLFW_KEY_RIGHT_ALT;
        case Qt::Key_Meta:
            return GLFW_KEY_LEFT_SUPER;
        case Qt::Key_Menu:
            return GLFW_KEY_MENU;
        default:
            return GLFW_KEY_UNKNOWN;
    }
}

int32_t
neon::vulkan::QTApplication::qtToGLFWMouseButton(Qt::MouseButton button) {
    switch (button) {
        case Qt::LeftButton:
            return GLFW_MOUSE_BUTTON_1;
        case Qt::RightButton:
            return GLFW_MOUSE_BUTTON_2;
        case Qt::MiddleButton:
            return GLFW_MOUSE_BUTTON_3;
        case Qt::ExtraButton4:
            return GLFW_MOUSE_BUTTON_4;
        case Qt::ExtraButton5:
            return GLFW_MOUSE_BUTTON_5;
        case Qt::ExtraButton6:
            return GLFW_MOUSE_BUTTON_6;
        case Qt::ExtraButton7:
            return GLFW_MOUSE_BUTTON_7;
        default:
            return -1;
    }
}

