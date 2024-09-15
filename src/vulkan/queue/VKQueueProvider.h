//
// Created by gaeqs on 6/08/24.
//

#ifndef VKQUEUEPROVIDER_H
#define VKQUEUEPROVIDER_H

#include <vector>
#include <mutex>
#include <condition_variable>

#include <vulkan/vulkan.h>

#include <vulkan/queue/VKQueueFamilyCollection.h>

namespace neon::vulkan {
    class VKQueueHolder;


    class VKQueueProvider {
        struct UsedQueue {
            uint32_t index;
            VkQueue queue;
            uint32_t amount;
        };

        friend class VKQueueHolder;

        VkDevice _device;
        VKQueueFamilyCollection _families;

        std::mutex _anyMutex;
        std::condition_variable _anyCondition;

        std::vector<std::mutex> _mutexes;
        std::vector<std::condition_variable> _conditions;

        std::vector<std::vector<uint32_t>> _availableQueues;
        std::vector<std::unordered_map<std::thread::id, UsedQueue>> _usedQueues;


        void freeQueue(uint32_t family, uint32_t index);

    public:
        VKQueueProvider(VkDevice device,
                        VKQueueFamilyCollection families,
                        const std::vector<uint32_t>& presentQueues);

        [[nodiscard]] VkDevice getDevice() const;

        [[nodiscard]] const VKQueueFamilyCollection& getFamilies() const;

        VKQueueHolder fetchQueue(uint32_t familyIndex);

        VKQueueHolder fetchCompatibleQueue(
            const VKQueueFamily::Capabilities& capabilities);

        bool markUsed(std::thread::id thread, uint32_t family, uint32_t index);
    };

    class VKQueueHolder {
        VKQueueProvider* _provider;
        VkQueue _queue;

        uint32_t _family;
        uint32_t _index;
        bool _valid;

    public:
        VKQueueHolder(const VKQueueHolder& other) = delete;

        VKQueueHolder();

        VKQueueHolder(VKQueueProvider* provider,
                      VkQueue queue,
                      uint32_t family,
                      uint32_t index);

        VKQueueHolder(VKQueueHolder&& move) noexcept;

        ~VKQueueHolder();

        [[nodiscard]] VkQueue getQueue() const;

        [[nodiscard]] uint32_t getFamily() const;

        [[nodiscard]] uint32_t getIndex() const;

        [[nodiscard]] bool isValid() const;

        VKQueueHolder& operator=(VKQueueHolder&& move) noexcept;
    };
}


#endif //VKQUEUEPROVIDER_H
