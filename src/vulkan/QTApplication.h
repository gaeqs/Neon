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

#include <QVulkanWindowRenderer>

#include <vulkan/AbstractVKApplication.h>

namespace neon::vulkan {

    class QTApplication : public QVulkanWindowRenderer,
                          public QVulkanWindow,
                          public AbstractVKApplication {

        using TimeStamp = std::chrono::time_point<
                std::chrono::system_clock,
                std::chrono::nanoseconds>;

        Application* _application;
        std::function<void(QTApplication*)> _onInit;

        VKThreadSafeQueue _graphicsQueue;
        mutable std::unique_ptr<CommandPool> _commandPool;
        VKQueueFamilyIndices _indices;

        FrameInformation _currentFrameInformation;
        TimeStamp _lastFrameTime;
        float _lastFrameProcessTime;

        mutable std::unique_ptr<CommandBuffer> _currentCommandBuffer;
        uint32_t _swapChainCount;
        bool _recording;

        VkDescriptorPool _imGuiPool;

    public:

        QTApplication();

        ~QTApplication() override;

        void setInitializationFunction(
                std::function<void(QTApplication*)> func);

        void init(Application* application) override;

        [[nodiscard]] rush::Vec2i getWindowSize() const override;

        [[nodiscard]] FrameInformation
        getCurrentFrameInformation() const override;

        [[nodiscard]] CommandBuffer* getCurrentCommandBuffer() const override;

        void lockMouse(bool lock) override;

        Result<uint32_t, std::string> startGameLoop() override;

        void renderFrame(Room* room) override;

        [[nodiscard]] VkInstance getInstance() const override;

        [[nodiscard]] VkDevice getDevice() const override;

        [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const override;

        [[nodiscard]] VKThreadSafeQueue& getGraphicsQueue() override;

        [[nodiscard]] VkFormat getSwapChainImageFormat() const override;

        [[nodiscard]] VkFormat getDepthImageFormat() const override;

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

        [[nodiscard]] VKQueueFamilyIndices getFamilyIndices() const override;

        void preInitResources() override;

        void initResources() override;

        void initSwapChainResources() override;

        void releaseSwapChainResources() override;

        void releaseResources() override;

        void startNextFrame() override;

        void physicalDeviceLost() override;

        void logicalDeviceLost() override;

        QVulkanWindowRenderer* createRenderer() override;

    protected:

        void mouseMoveEvent(QMouseEvent* event) override;

        void mousePressEvent(QMouseEvent* event) override;

        void mouseReleaseEvent(QMouseEvent*) override;

        void keyPressEvent(QKeyEvent* event) override;

        void keyReleaseEvent(QKeyEvent* event) override;

        void wheelEvent(QWheelEvent* event) override;

    private:

        void initImGui();

        void setupImGUIFrame() const;

        static int32_t qtToGLFWKey(Qt::Key key);

        static int32_t qtToGLFWMouseButton(Qt::MouseButton button);

    };
}

#endif //VISIMPL_QTAPPLICATION_H
