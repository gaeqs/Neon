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

#ifndef VISIMPL_QTAPPLICATION_H
#define VISIMPL_QTAPPLICATION_H

#ifdef USE_QT

    #include <QVulkanWindowRenderer>

    #include <QPointer>

    #include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan
{

    class QTApplication;

    /**
     * @brief This class is the bridge between a QVulkanWindow and a QTApplication.
     *
     * This class inherits from QVulkanWindowRenderer and QVulkanWindow.
     * It handles all the Vulkan-related calls from the QT Window, redirecting
     * them to the QTApplication.
     *
     * It also handles all the QT events.
     */
    class QTApplicationHandler : public QVulkanWindowRenderer, public QVulkanWindow
    {
        QTApplication* _application;

      public:
        /**
         * @brief Constructs a new QTApplicationHandler.
         * @param application the application to link to.
         */
        explicit QTApplicationHandler(QTApplication* application);

        /**
         * @brief Invalidates the handler.
         * This method is called when the application is destroyed.
         */
        void invalidate();

        void preInitResources() override;

        void initResources() override;

        void initSwapChainResources() override;

        void releaseSwapChainResources() override;

        void releaseResources() override;

        void startNextFrame() override;

        void physicalDeviceLost() override;

        void logicalDeviceLost() override;

        bool eventFilter(QObject* watched, QEvent* event) override;

        void mouseMoveEvent(QMouseEvent*) override;

        void mousePressEvent(QMouseEvent*) override;

        void mouseReleaseEvent(QMouseEvent*) override;

        void wheelEvent(QWheelEvent*) override;

        QVulkanWindowRenderer* createRenderer() override;
    };

    /**
     * @brief An ApplicationImplementation that uses QT as a backend.
     *
     * This class uses a QTApplicationHandler to manage the window and the Vulkan context.
     * The game loop is managed by QT's event loop.
     */
    class QTApplication : public AbstractVKApplication
    {
        using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;

        QPointer<QTApplicationHandler> _handler;

        Application* _application;
        std::function<void(QTApplication*)> _onInit;

        std::unique_ptr<VKQueueFamilyCollection> _queueFamilies;
        std::vector<float> _queuePriorities;
        std::vector<uint32_t> _presentQueues;
        VKPhysicalDevice _physicalDevice;
        std::unique_ptr<VKDevice> _device;

        std::unique_ptr<CommandManager> _commandManager;
        CommandPoolHolder _commandPool;
        VKResourceBin _bin;

        FrameInformation _currentFrameInformation;
        TimeStamp _lastFrameTime;
        float _lastFrameProcessTime;

        CommandBuffer* _currentCommandBuffer;
        uint32_t _swapChainCount;
        bool _recording;

        VkDescriptorPool _imGuiPool;

        void initImGui();

        void setupImGUIFrame() const;

        static int32_t qtToGLFWKey(Qt::Key key);

        static int32_t qtToGLFWMouseButton(Qt::MouseButton button);

      public:
        QTApplication(const QTApplication& other) = delete;

        /**
         * @brief Creates a new QTApplication.
         * @param instance the Vulkan instance to use.
         */
        explicit QTApplication(QVulkanInstance* instance);

        ~QTApplication() override;

        /**
         * @brief Returns the handler of this application.
         * The handler is the QVulkanWindow that manages the window.
         * @return the handler.
         */
        QVulkanWindow* getHandler();

        /**
         * @brief Sets the function to be called when the application is initialized.
         * @param func the function.
         */
        void setInitializationFunction(std::function<void(QTApplication*)> func);

        void init(Application* application) override;

        [[nodiscard]] const CommandManager& getCommandManager() const override;

        [[nodiscard]] CommandManager& getCommandManager() override;

        [[nodiscard]] rush::Vec2i getWindowSize() const override;

        [[nodiscard]] FrameInformation getCurrentFrameInformation() const override;

        [[nodiscard]] CommandBuffer* getCurrentCommandBuffer() const override;

        void lockMouse(bool lock) override;

        Result<uint32_t, std::string> startGameLoop() override;

        void renderFrame(Room* room) override;

        [[nodiscard]] VkInstance getInstance() const override;

        [[nodiscard]] VKDevice* getDevice() const override;

        [[nodiscard]] const VKPhysicalDevice& getPhysicalDevice() const override;

        [[nodiscard]] VkFormat getSwapChainImageFormat() const override;

        [[nodiscard]] TextureFormat getDepthImageFormat() const override;

        [[nodiscard]] CommandPool* getCommandPool() const override;

        [[nodiscard]] VkSwapchainKHR getSwapChain() const override;

        [[nodiscard]] uint32_t getMaxFramesInFlight() const override;

        [[nodiscard]] uint32_t getCurrentFrame() const override;

        [[nodiscard]] uint32_t getCurrentSwapChainImage() const override;

        [[nodiscard]] uint32_t getSwapChainCount() const override;

        [[nodiscard]] VkExtent2D getSwapChainExtent() const override;

        [[nodiscard]] VkDescriptorPool getImGuiPool() const override;

        [[nodiscard]] bool isRecordingCommandBuffer() const override;

        [[nodiscard]] Application* getApplication() const override;

        [[nodiscard]] VkQueue getGraphicsQueue() override;

        // region QT-related methods.
        // These methods are called by the QTApplicationHandler.

        void preInitResources();

        void initResources();

        void initSwapChainResources();

        void releaseSwapChainResources();

        void releaseResources();

        void startNextFrame();

        void physicalDeviceLost();

        void logicalDeviceLost();

        // endregion

        const ApplicationCreateInfo& getCreationInfo() const override;

        bool isInModalMode() const override;

        void setModalMode(bool modal) override;

        [[nodiscard]] bool isMainThread() const override;

        [[nodiscard]] VkFormat getVkDepthImageFormat() const override;

        [[nodiscard]] VKResourceBin* getBin() override;

        // region Event handlers
        // These methods are called by the QTApplicationHandler's event filter.

        void mouseMoveEvent(QMouseEvent* event);

        void mousePressEvent(QMouseEvent* event);

        void mouseReleaseEvent(QMouseEvent* event);

        void keyPressEvent(QKeyEvent* event);

        void keyReleaseEvent(QKeyEvent* event);

        void wheelEvent(QWheelEvent* event);

        // endregion
    };
} // namespace neon::vulkan

#endif

#endif // VISIMPL_QTAPPLICATION_H
