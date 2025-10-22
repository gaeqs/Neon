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

#include <neon/io/KeyboardEvent.h>
#include <vulkan/vulkan_core.h>
#ifdef USE_QT

    #include "QTApplication.h"

    #include <utility>
    #include <qthread.h>

    #include <neon/structure/Room.h>

    #include <QWheelEvent>
    #include <QSurface>
    #include <QVulkanFunctions>

    #include <vulkan/vulkan.h>
    #include <vulkan/util/VulkanConversions.h>
    #include <vulkan/util/VKUtil.h>

    #include <imgui.h>
    #include <imgui_impl_vulkan.h>
    #include <imgui_impl_glfw.cpp>
    #include <implot.h>

namespace
{
    bool debugFilter(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object,
                     size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage)
    {
        auto message = neon::MessageBuilder().group("vulkan").print(pMessage);
        if (flags & (VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)) {
            neon::warning().group("vulkan") << pMessage;
        } else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
            neon::error().group("vulkan") << pMessage;
        } else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
            neon::debug().group("vulkan") << pMessage;
        } else {
            neon::info().group("vulkan") << pMessage;
        }
        return true;
    }
} // namespace

namespace neon::vulkan
{
    QTApplication::QTApplication() :
        _application(nullptr),
        _currentFrameInformation(0, 0.016f, 0.0f),
        _lastFrameTime(std::chrono::high_resolution_clock::now()),
        _lastFrameProcessTime(0.0f),
        _currentCommandBuffer(nullptr),
        _swapChainCount(0),
        _recording(false),
        _imGuiPool(VK_NULL_HANDLE)
    {
    }

    void QTApplication::init(neon::Application* application)
    {
        _application = application;
    }

    rush::Vec2i QTApplication::getWindowSize() const
    {
        QSize s = size();
        return {s.width(), s.height()};
    }

    neon::FrameInformation QTApplication::getCurrentFrameInformation() const
    {
        return _currentFrameInformation;
    }

    neon::CommandBuffer* QTApplication::getCurrentCommandBuffer() const
    {
        return _currentCommandBuffer;
    }

    void QTApplication::lockMouse(bool lock)
    {
        // TODO
    }

    neon::Result<uint32_t, std::string> QTApplication::startGameLoop()
    {
        return {"QTApplicaiton doesn't contain a GameLoop, as Qt already has one."};
    }

    void QTApplication::renderFrame(neon::Room* room)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - _lastFrameTime;
        _lastFrameTime = now;

        float seconds = static_cast<float>(duration.count()) * 1e-9f;

        _currentCommandBuffer = _commandPool.getPool().beginCommandBuffer(true);
        _currentFrameInformation = {_currentFrameInformation.currentFrame + 1, seconds, _lastFrameProcessTime, false};

        {
            DEBUG_PROFILE_ID(_application->getProfiler(), tasks, "tasks");
            _application->getTaskRunner().flushMainThreadTasks();
        }

        setupImGUIFrame();

        _application->getRender()->checkFrameBufferRecreationConditions();

        _bin.flush();
        if (room != nullptr) {
            room->update(_currentFrameInformation.currentDeltaTime);
            room->preDraw();
            _application->getRender()->render(room);
        }

        _currentCommandBuffer->end();
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        _currentCommandBuffer->getImplementation().submit(nullptr, nullptr, waitStages);

        frameReady();
        requestUpdate();

        now = std::chrono::high_resolution_clock::now();
        auto processTime = now - _lastFrameTime;
        _lastFrameProcessTime = static_cast<float>(processTime.count()) * 1e-9f;
    }

    VkInstance QTApplication::getInstance() const
    {
        return vulkanInstance()->vkInstance();
    }

    VKDevice* QTApplication::getDevice() const
    {
        return _device.get();
    }

    const VKPhysicalDevice& QTApplication::getPhysicalDevice() const
    {
        return _physicalDevice;
    }

    VkFormat QTApplication::getSwapChainImageFormat() const
    {
        return colorFormat();
    }

    TextureFormat QTApplication::getDepthImageFormat() const
    {
        return conversions::textureFormat(depthStencilFormat());
    }

    CommandPool* QTApplication::getCommandPool() const
    {
        return &_commandPool.getPool();
    }

    VkSwapchainKHR QTApplication::getSwapChain() const
    {
        neon::error() << "QTApplication doesn't expose a swap chain!";
        return nullptr;
    }

    uint32_t QTApplication::getMaxFramesInFlight() const
    {
        return MAX_CONCURRENT_FRAME_COUNT;
    }

    uint32_t QTApplication::getCurrentFrame() const
    {
        return concurrentFrameCount();
    }

    uint32_t QTApplication::getCurrentSwapChainImage() const
    {
        return swapChainImageCount();
    }

    uint32_t QTApplication::getSwapChainCount() const
    {
        return _swapChainCount;
    }

    VkExtent2D QTApplication::getSwapChainExtent() const
    {
        QSize s = swapChainImageSize();
        return {static_cast<uint32_t>(s.width()), static_cast<uint32_t>(s.height())};
    }

    VkDescriptorPool QTApplication::getImGuiPool() const
    {
        return _imGuiPool;
    }

    bool QTApplication::isRecordingCommandBuffer() const
    {
        return _recording;
    }

    neon::Application* QTApplication::getApplication() const
    {
        return _application;
    }

    VkQueue QTApplication::getGraphicsQueue()
    {
        return graphicsQueue();
    }

    void QTApplication::preInitResources()
    {
        vulkanInstance()->installDebugOutputFilter(debugFilter);

        _queueFamilies =
            std::make_unique<VKQueueFamilyCollection>(physicalDevice(), vulkanInstance()->surfaceForWindow(this));

        uint32_t maxQueues = 0;
        for (auto family : _queueFamilies->getFamilies()) {
            maxQueues = std::max(family.getCount(), maxQueues);
        }
        _queuePriorities.resize(maxQueues, 1.0f);

        _presentQueues.resize(_queueFamilies->getFamilies().size());

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve(_queueFamilies->getFamilies().size());

        for (auto family : _queueFamilies->getFamilies()) {
            VkDeviceQueueCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.queueFamilyIndex = family.getIndex();
            info.queueCount = family.getCount();
            info.pQueuePriorities = _queuePriorities.data();
            queueCreateInfos.push_back(info);
            _presentQueues[family.getIndex()] = family.getCount();
        }

        setQueueCreateInfoModifier([queueCreateInfos](const VkQueueFamilyProperties* prop, uint32_t amount,
                                                      QVector<VkDeviceQueueCreateInfo>& vec) {
            vec.clear();
            for (auto& info : queueCreateInfos) {
                vec.push_back(info);
            }
        });
    }

    void QTApplication::initResources()
    {
        _physicalDevice = VKPhysicalDevice(physicalDevice(), nullptr);
        _device = std::make_unique<VKDevice>(vulkanInstance()->vkInstance(), physicalDevice(), device(),
                                             *_queueFamilies, _physicalDevice.getFeatures(), _presentQueues);

        neon::debug() << "Family: " << graphicsQueueFamilyIndex();

        // Fetch queue index:
        size_t family = graphicsQueueFamilyIndex();
        VkQueue queue = graphicsQueue();
        VkQueue fetchedQueue;
        size_t index = 0;
        do {
            vkGetDeviceQueue(_device->getDeviceWithoutHolding(), family, index, &fetchedQueue);
            ++index;
        } while (queue != fetchedQueue);
        --index;

        neon::debug() << "Index: " << index;

        _device->getQueueProvider()->markUsed(std::this_thread::get_id(), graphicsQueueFamilyIndex(), index);
        _commandPool = _application->getCommandManager().fetchCommandPool();

        initImGui();
        if (_onInit) {
            _onInit(this);
        }
    }

    void QTApplication::initSwapChainResources()
    {
        ++_swapChainCount;
    }

    void QTApplication::releaseSwapChainResources()
    {
    }

    void QTApplication::releaseResources()
    {
        _bin.waitAndFlush();

        if (ImGui::GetIO().BackendRendererUserData != nullptr) {
            ImGui_ImplVulkan_Shutdown();
        }

        ImPlot::DestroyContext();
        ImGui::DestroyContext();


        vkDestroyDescriptorPool(_device->hold(), _imGuiPool, nullptr);
        _commandPool = CommandPoolHolder();

        _device = nullptr;
    }

    void QTApplication::startNextFrame()
    {
        if (_application != nullptr) {
            _recording = true;
            renderFrame(_application->getRoom().get());
            _recording = false;
        }
    }

    void QTApplication::physicalDeviceLost()
    {
        neon::debug() << "Physical device lost";
    }

    void QTApplication::logicalDeviceLost()
    {
        neon::debug() << "Logical device lost";
    }

    QVulkanWindowRenderer* QTApplication::createRenderer()
    {
        return this;
    }

    const ApplicationCreateInfo& QTApplication::getCreationInfo() const
    {
        throw std::runtime_error("QTApplication doesn't contain a creation info!");
    }

    bool QTApplication::isInModalMode() const
    {
        return _currentFrameInformation.modalMode;
    }

    void QTApplication::setModalMode(bool modal)
    {
    }

    bool QTApplication::isMainThread() const
    {
        return QThread::isMainThread();
    }

    VkFormat QTApplication::getVkDepthImageFormat() const
    {
        return depthStencilFormat();
    }

    VKResourceBin* QTApplication::getBin()
    {
        return &_bin;
    }

    bool QTApplication::eventFilter(QObject* watched, QEvent* event)
    {
        if (event->type() == QEvent::KeyPress) {
            keyPressEvent(static_cast<QKeyEvent*>(event));
            return true;
        }
        if (event->type() == QEvent::KeyRelease) {
            keyReleaseEvent(static_cast<QKeyEvent*>(event));
            return true;
        }
        return QVulkanWindow::eventFilter(watched, event);
    }

    void QTApplication::setInitializationFunction(std::function<void(QTApplication*)> func)
    {
        _onInit = std::move(func);
    }

    void QTApplication::mouseMoveEvent(QMouseEvent* event)
    {
        auto [x, y] = event->position();
        _application->invokeCursorPosEvent(x, y);
        ImGui::GetIO().AddMousePosEvent(static_cast<float>(x), static_cast<float>(y));
    }

    void QTApplication::mousePressEvent(QMouseEvent* event)
    {
        auto qtMod = event->modifiers();
        int modifier = 0;
        if (qtMod & Qt::ShiftModifier) {
            modifier |= (int)neon::KeyboardModifier::SHIFT;
        }
        if (qtMod & Qt::ControlModifier) {
            modifier |= (int)neon::KeyboardModifier::CONTROL;
        }
        if (qtMod & Qt::AltModifier) {
            modifier |= (int)neon::KeyboardModifier::ALT;
        }
        if (qtMod & Qt::MetaModifier) {
            modifier |= (int)neon::KeyboardModifier::SUPER;
        }

        int glfwButton = qtToGLFWMouseButton(event->button());

        auto& io = ImGui::GetIO();
        io.AddMouseButtonEvent(glfwButton, true);
        if (!io.WantCaptureMouse) {
            _application->invokeMouseButtonEvent(glfwButton, GLFW_PRESS, modifier);
        }
    }

    void QTApplication::mouseReleaseEvent(QMouseEvent* event)
    {
        auto qtMod = event->modifiers();
        int modifier = 0;
        if (qtMod & Qt::ShiftModifier) {
            modifier |= (int)neon::KeyboardModifier::SHIFT;
        }
        if (qtMod & Qt::ControlModifier) {
            modifier |= (int)neon::KeyboardModifier::CONTROL;
        }
        if (qtMod & Qt::AltModifier) {
            modifier |= (int)neon::KeyboardModifier::ALT;
        }
        if (qtMod & Qt::MetaModifier) {
            modifier |= (int)neon::KeyboardModifier::SUPER;
        }

        int glfwButton = qtToGLFWMouseButton(event->button());

        auto& io = ImGui::GetIO();
        io.AddMouseButtonEvent(glfwButton, false);
        if (!io.WantCaptureMouse) {
            _application->invokeMouseButtonEvent(glfwButton, GLFW_RELEASE, modifier);
        }
    }

    void QTApplication::keyPressEvent(QKeyEvent* event)
    {
        auto qtMod = event->modifiers();
        int modifier = 0;
        if (qtMod & Qt::ShiftModifier) {
            modifier |= (int)neon::KeyboardModifier::SHIFT;
        }
        if (qtMod & Qt::ControlModifier) {
            modifier |= (int)neon::KeyboardModifier::CONTROL;
        }
        if (qtMod & Qt::AltModifier) {
            modifier |= (int)neon::KeyboardModifier::ALT;
        }
        if (qtMod & Qt::MetaModifier) {
            modifier |= (int)neon::KeyboardModifier::SUPER;
        }

        int glfwKey = qtToGLFWKey((Qt::Key)event->key());
        auto& io = ImGui::GetIO();
        io.AddKeyEvent(ImGui_ImplGlfw_KeyToImGuiKey(glfwKey, event->nativeScanCode()), true);
        if (!io.WantCaptureKeyboard) {
            _application->invokeKeyEvent(glfwKey, event->nativeScanCode(), event->isAutoRepeat() ? 2 : 1, modifier);
        }
    }

    void QTApplication::keyReleaseEvent(QKeyEvent* event)
    {
        auto qtMod = event->modifiers();
        int modifier = 0;
        if (qtMod & Qt::ShiftModifier) {
            modifier |= (int)neon::KeyboardModifier::SHIFT;
        }
        if (qtMod & Qt::ControlModifier) {
            modifier |= (int)neon::KeyboardModifier::CONTROL;
        }
        if (qtMod & Qt::AltModifier) {
            modifier |= (int)neon::KeyboardModifier::ALT;
        }
        if (qtMod & Qt::MetaModifier) {
            modifier |= (int)neon::KeyboardModifier::SUPER;
        }

        int glfwKey = qtToGLFWKey((Qt::Key)event->key());
        auto& io = ImGui::GetIO();
        io.AddKeyEvent(ImGui_ImplGlfw_KeyToImGuiKey(glfwKey, event->nativeScanCode()), false);
        if (!io.WantCaptureKeyboard) {
            _application->invokeKeyEvent(glfwKey, event->nativeScanCode(), 0, modifier);
        }
    }

    void QTApplication::wheelEvent(QWheelEvent* event)
    {
        // https://doc.qt.io/qt-6/qwheelevent.html#angleDelta
        constexpr double DELTAS_PER_STEP = 120.0;

        auto x = static_cast<double>(event->angleDelta().x()) / DELTAS_PER_STEP;
        auto y = static_cast<double>(event->angleDelta().y()) / DELTAS_PER_STEP;
        auto& io = ImGui::GetIO();
        io.AddMouseWheelEvent(static_cast<float>(x), static_cast<float>(y));
        if (!io.WantCaptureMouse) {
            _application->invokeScrollEvent(x, y);
        }
    }

    void QTApplication::initImGui()
    {
        VkDescriptorPoolSize pool_sizes[] = {
            {               VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {         VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {         VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {  VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {  VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {      VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
        };

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        auto* fun = vulkanInstance()->deviceFunctions(device());
        if (fun->vkCreateDescriptorPool(device(), &pool_info, nullptr, &_imGuiPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to init ImGui!");
        }

        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    void QTApplication::setupImGUIFrame() const
    {
        auto size = getWindowSize().cast<float>();

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(size.x(), size.y());
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        io.DeltaTime = _currentFrameInformation.currentDeltaTime;

        ImGui_ImplVulkan_NewFrame();
        ImGui::NewFrame();
    }

    int32_t QTApplication::qtToGLFWKey(Qt::Key key)
    {
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

    int32_t QTApplication::qtToGLFWMouseButton(Qt::MouseButton button)
    {
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
} // namespace neon::vulkan

#endif
