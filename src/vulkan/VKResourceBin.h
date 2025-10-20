//
// Created by gaeqs on 16/04/2025.
//

#ifndef VKRESOURCEBIN_H
#define VKRESOURCEBIN_H

#include <functional>
#include <mutex>
#include <vector>
#include <memory>
#include <neon/render/buffer/CommandBufferRun.h>
#include <vulkan/vulkan_core.h>

namespace neon
{

    /**
     * Manages Vulkan resources with a binning strategy to handle deferred destruction.
     * Resources are grouped and destroyed only after their associated command buffer runs are completed.
     */
    class VKResourceBin
    {
        struct Entry
        {
            VkDevice device;
            std::vector<std::shared_ptr<CommandBufferRun>> runs;
            std::function<void()> deleteFunction;
        };

        std::vector<Entry> _entries;
        std::mutex _mutex;

      public:
        ~VKResourceBin();

        /**
         * Schedules a resource for deferred destruction.
         * The resource will not be destroyed until all associated command buffer runs have finished.
         *
         * @param device The Vulkan device associated with the resource.
         * @param runs A vector of shared pointers to CommandBufferRun objects. These represent the command buffer runs
         *             whose completion is required before the resource can be destroyed.
         * @param deleteFunction The callback function that performs the destruction of the resource.
         */
        void destroyLater(VkDevice device, std::vector<std::shared_ptr<CommandBufferRun>> runs,
                          std::function<void()> deleteFunction);

        /**
         * Clears stored resources in the VKResourceBin that are no longer needed.
         *
         * This method iterates through all stored entries and evaluates whether their associated CommandBufferRun
         * objects have completed execution. If all associated runs of an entry have finished, the entry's associated
         * delete function is invoked to clean up the corresponding resource, and the entry is removed from the list.
         *
         * The method ensures thread safety, allowing safe concurrent access to the
         * underlying resource bin in a multithreaded environment.
         *
         * This function is typically invoked during object destruction or regular application updates to release
         * resources in a controlled and timely manner.
         */
        void flush();

        /**
         * Clears all resources inside this VKResourceBin, waiting for the GPU to free them.
         *
         * This function is typically invoked when the VKResourceBin is being destroyed.
         */
        void waitAndFlush();

        /**
         * Schedules a resource for deferred destruction.
         * The resource will not be destroyed until all associated command buffer runs have finished.
         *
         * @param device The Vulkan device associated with the resource.
         * @param runs A vector of shared pointers to CommandBufferRun objects. These represent the command buffer runs
         *             whose completion is required before the resource can be destroyed.
         * @param destroyFn A callable that handles the actual destruction of the resource.
         */
        template<typename T>
        void destroyLater(VkDevice device, std::vector<std::shared_ptr<CommandBufferRun>> runs, T* resource,
                          void (*destroyFn)(VkDevice, T*, const VkAllocationCallbacks*))
        {
            destroyLater(device, std::move(runs), [=] { destroyFn(device, resource, nullptr); });
        }
    };

} // namespace neon

#endif // VKRESOURCEBIN_H
