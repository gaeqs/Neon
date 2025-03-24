//
// Created by gaelr on 7/06/23.
//

#ifndef VISIMPL_ABSTRACTVKAPPLICATION_H
#define VISIMPL_ABSTRACTVKAPPLICATION_H

#include <neon/structure/Application.h>
#include <neon/render/buffer/CommandPool.h>
#include <vulkan/vulkan.h>
#include <vulkan/device/VKDevice.h>
#include <vulkan/device/VKPhysicalDevice.h>

namespace neon
{
    class Application;
}

namespace neon::vulkan
{
    class AbstractVKApplication : public ApplicationImplementation
    {
      public:
        [[nodiscard]] virtual Application* getApplication() const = 0;

        [[nodiscard]] virtual VkInstance getInstance() const = 0;

        [[nodiscard]] virtual VKDevice* getDevice() const = 0;

        [[nodiscard]] virtual const VKPhysicalDevice& getPhysicalDevice() const = 0;

        [[nodiscard]] virtual VkQueue getGraphicsQueue() = 0;

        [[nodiscard]] virtual VkFormat getSwapChainImageFormat() const = 0;

        [[nodiscard]] virtual VkFormat getDepthImageFormat() const = 0;

        [[nodiscard]] virtual CommandPool* getCommandPool() const = 0;

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
} // namespace neon::vulkan

#endif //VISIMPL_ABSTRACTVKAPPLICATION_H
