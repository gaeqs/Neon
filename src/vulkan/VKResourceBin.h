//
// Created by gaeqs on 16/04/2025.
//

#ifndef VKRESOURCEBIN_H
#define VKRESOURCEBIN_H

#include <functional>
#include <mutex>
#include <vector>
#include <neon/render/buffer/CommandBufferRun.h>
#include <vulkan/vulkan_core.h>

namespace neon
{

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

        void destroyLater(VkDevice device, std::vector<std::shared_ptr<CommandBufferRun>> runs,
                          std::function<void()> deleteFunction);

        void flush();

        template<typename T>
        void destroyLater(VkDevice device, std::vector<std::shared_ptr<CommandBufferRun>> runs, T* resource,
                          void (*destroyFn)(VkDevice, T*, const VkAllocationCallbacks*))
        {
            destroyLater(device, std::move(runs), [=] { destroyFn(device, resource, nullptr); });
        }
    };

} // namespace neon

#endif // VKRESOURCEBIN_H
