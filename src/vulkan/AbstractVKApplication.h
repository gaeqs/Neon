//
// Created by gaelr on 7/06/23.
//

#ifndef VISIMPL_ABSTRACTVKAPPLICATION_H
#define VISIMPL_ABSTRACTVKAPPLICATION_H

#include <engine/Application.h>

#include <vulkan/vulkan.h>

namespace neon::vulkan {

    class AbstractVKApplication : public ApplicationImplementation {

    public:

        [[nodiscard]] virtual VkInstance getInstance() const = 0;

        [[nodiscard]] virtual VkDevice getDevice() const = 0;

        [[nodiscard]] virtual VkPhysicalDevice getPhysicalDevice() const = 0;

        [[nodiscard]] virtual VkQueue getGraphicsQueue() const = 0;

        [[nodiscard]] virtual VkFormat getSwapChainImageFormat() const = 0;

        [[nodiscard]] virtual VkFormat getDepthImageFormat() const = 0;

        [[nodiscard]] virtual VkCommandPool getCommandPool() const = 0;

        [[nodiscard]] virtual VkSwapchainKHR getSwapChain() const = 0;

        [[nodiscard]] virtual uint32_t getMaxFramesInFlight() const = 0;

        /**
         * Returns the current frame index.
         * This number's value is in the range [0, getMaxFramesInFlight()).
         * @return the index.
         */
        [[nodiscard]] virtual uint32_t getCurrentFrame() const = 0;

        /**
         * Returns the current index of the swap chain image.
         * This may not be the same as the current frame index!
         * @return the index.
         */
        [[nodiscard]] virtual uint32_t getCurrentSwapChainImage() const = 0;

        /**
         * Return the number of times the swap chain has been recreated.
         * @return the number of times it has been recreated.
         */
        [[nodiscard]] virtual uint32_t getSwapChainCount() const = 0;

        /**
         * Returns the extent of the current swap chain.
         * @return the extent.
         */
        [[nodiscard]] virtual VkExtent2D getSwapChainExtent() const = 0;

        /**
         * Returns the VkDescriptorPool used by ImGUI for rendering.
         * @return the ImGUI descriptor pool.
         */
        [[nodiscard]] virtual VkDescriptorPool getImGuiPool() const = 0;

        /**
         * Returns whether the current command buffer is being recorded.
         * @return whether the current command buffer is recording.
         */
        [[nodiscard]] virtual bool isRecordingCommandBuffer() const = 0;

    };
}

#endif //VISIMPL_ABSTRACTVKAPPLICATION_H
