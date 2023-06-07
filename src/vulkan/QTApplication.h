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

namespace neon::vulkan {

    class QTApplication : public QVulkanWindowRenderer {

    public:

        void preWindowCreation();

        void postWindowCreation(GLFWwindow* window);

        bool preUpdate(Profiler& profiler);

        void endDraw(Profiler& pofiler);

        void finishLoop();

        void internalForceSizeValues(int32_t width, int32_t height);

        void internalKeyEvent(int key, int scancode, int action, int mods);

        void internalCursorPosEvent(double x, double y);

        [[nodiscard]] uint32_t getCurrentFrame() const;

        [[nodiscard]] uint32_t getCurrentSwapChainImage() const;

        [[nodiscard]] uint32_t getMaxFramesInFlight() const;

        [[nodiscard]] VkInstance getInstance() const;

        [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const;

        [[nodiscard]] VkDevice getDevice() const;

        [[nodiscard]] VkQueue getGraphicsQueue() const;

        [[nodiscard]] VkQueue getPresentQueue() const;

        [[nodiscard]] VkSurfaceKHR getSurface() const;

        [[nodiscard]] VkSwapchainKHR getSwapChain() const;

        [[nodiscard]] VkFormat getSwapChainImageFormat() const;

        [[nodiscard]] VkFormat getDepthImageFormat() const;

        [[nodiscard]] const VkExtent2D& getSwapChainExtent() const;

        [[nodiscard]] uint32_t getSwapChainCount() const;

        [[nodiscard]] VkCommandPool getCommandPool() const;

        [[nodiscard]] CommandBuffer* getCurrentCommandBuffer() const;

        [[nodiscard]] VkDescriptorPool getImGuiPool() const;

        [[nodiscard]] bool isRecordingCommandBuffer() const;

        void setRoom(const std::shared_ptr<Room>& room);

    };
}

#endif //VISIMPL_QTAPPLICATION_H
