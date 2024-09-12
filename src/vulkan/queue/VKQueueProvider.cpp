//
// Created by gaeqs on 6/08/24.
//

#include "VKQueueProvider.h"

namespace neon::vulkan {
    void VKQueueProvider::freeQueue(uint32_t family, uint32_t index) {
        if(_availableQueues.size() <= family) {
            std::cerr << "[NEON] Invalid queue family index: " << family <<
                    "." << std::endl;
            return;
        }

        std::thread::id threadId = std::this_thread::get_id();
        std::unique_lock lock(_mutexes[family]);

        // Check if the thread is using a queue of the same family
        auto& map = _usedQueues[family];
        auto entry = map.find(threadId);
        if(entry != map.end()) {
            if(entry->second.amount < 2) {
                map.erase(entry);
                _availableQueues[family].push_back(index);
                _conditions[family].notify_one();
                _anyCondition.notify_one();
            } else {
                entry->second.amount--;
            }
        }
    }

    VKQueueProvider::
    VKQueueProvider(VkDevice device,
                    VKQueueFamilyCollection families,
                    const std::vector<uint32_t>& presentQueues)
        : _device(device),
          _families(std::move(families)),
          _mutexes(_families.getFamilies().size()),
          _conditions(_families.getFamilies().size()) {
        _usedQueues.resize(_families.getFamilies().size());
        _availableQueues.reserve(presentQueues.size());
        for(uint32_t count: presentQueues) {
            auto& vec = _availableQueues.emplace_back();
            vec.reserve(count);
            for(uint32_t i = 0; i < count; ++i) {
                vec.push_back(i);
            }
        }
    }

    VkDevice VKQueueProvider::getDevice() const {
        return _device;
    }

    VKQueueFamilyCollection VKQueueProvider::getFamilies() const {
        return _families;
    }

    VKQueueHolder VKQueueProvider::fetchQueue(uint32_t familyIndex) {
        if(_availableQueues.size() <= familyIndex) {
            std::cerr << "[NEON] Invalid queue family index: " << familyIndex <<
                    "." << std::endl;
            return {};
        }

        std::thread::id threadId = std::this_thread::get_id();
        std::unique_lock lock(_mutexes[familyIndex]);

        // Check if the thread is using a queue of the same family
        auto& map = _usedQueues[familyIndex];
        auto entry = map.find(threadId);
        if(entry != map.end()) {
            entry->second.amount++;
            return {
                this,
                entry->second.queue,
                familyIndex,
                entry->second.index
            };
        }

        auto& queues = _availableQueues[familyIndex];

        _conditions[familyIndex].wait(lock, [&] {
            return !queues.empty();
        });

        uint32_t index = queues.back();
        queues.pop_back();

        VkQueue queue;
        vkGetDeviceQueue(_device, familyIndex, index, &queue);

        map[threadId] = UsedQueue(index, queue, 1);

        return {this, queue, familyIndex, index};
    }

    VKQueueHolder VKQueueProvider::fetchCompatibleQueue(
        const VKQueueFamily::Capabilities& capabilities) {
        std::vector<const VKQueueFamily*> families;
        _families.getAllCompatibleQueueFamilies(capabilities, families);

        if(families.empty()) return {};
        if(families.size() == 1) return fetchQueue(families[0]->getIndex());

        std::thread::id threadId = std::this_thread::get_id();
        std::unique_lock lock(_anyMutex);

        while(true) {
            for(auto& family: families) {
                uint32_t familyIndex = family->getIndex();
                std::unique_lock queueLock(_mutexes[familyIndex]);

                // Check if the thread is using a queue of the same family
                auto& map = _usedQueues[familyIndex];
                auto entry = map.find(threadId);
                if(entry != map.end()) {
                    entry->second.amount++;
                    return {
                        this,
                        entry->second.queue,
                        familyIndex,
                        entry->second.index
                    };
                }

                auto& queues = _availableQueues[familyIndex];
                if(!queues.empty()) {
                    uint32_t index = queues.back();
                    queues.pop_back();

                    VkQueue queue;
                    vkGetDeviceQueue(_device, familyIndex, index, &queue);
                    map[threadId] = UsedQueue(index, queue, 1);
                    return {this, queue, familyIndex, index};
                }
            }

            _anyCondition.wait(lock);
        }
    }

    bool VKQueueProvider::markUsed(std::thread::id thread, uint32_t family,
                                   uint32_t index) {
        if(_availableQueues.size() <= family) {
            std::cerr << "[NEON] Invalid queue family index: " << family <<
                    "." << std::endl;
            return false;
        }

        std::unique_lock lock(_mutexes[family]);

        auto& map = _usedQueues[family];
        auto entry = map.find(thread);
        if(entry != map.end()) {
            if(entry->second.index == index) {
                entry->second.amount++;
                return true;
            }
            std::cerr << "[NEON] Cannot mark family " << family <<
                    ". Another queue index is assigned." << std::endl;
            return false;
        }

        auto& available = _availableQueues[family];
        auto pos = std::ranges::find(available, index);

        if(pos == available.end()) {
            std::cerr << "[NEON] Queue " << index
                    << " of family " << family <<
                    ". Is already being used." << std::endl;
            return false;
        }

        available.erase(pos);

        VkQueue queue;
        vkGetDeviceQueue(_device, index, index, &queue);

        map[thread] = UsedQueue(index, queue, 1);
        return true;
    }

    VKQueueHolder::VKQueueHolder()
        : _provider(nullptr),
          _queue(VK_NULL_HANDLE),
          _family(0),
          _index(0),
          _valid(false) {}

    VKQueueHolder::VKQueueHolder(VKQueueProvider* provider,
                                 VkQueue queue,
                                 uint32_t family,
                                 uint32_t index)
        : _provider(provider),
          _queue(queue),
          _family(family),
          _index(index),
          _valid(true) {}

    VKQueueHolder::VKQueueHolder(VKQueueHolder&& move) noexcept
        : _provider(move._provider),
          _queue(move._queue),
          _family(move._family),
          _index(move._index),
          _valid(move._valid) {
        move._valid = false;
    }

    VKQueueHolder::~VKQueueHolder() {
        if(_valid) {
            _provider->freeQueue(_family, _index);
        }
    }

    VkQueue VKQueueHolder::getQueue() const {
        return _queue;
    }

    uint32_t VKQueueHolder::getFamily() const {
        return _family;
    }

    uint32_t VKQueueHolder::getIndex() const {
        return _index;
    }

    bool VKQueueHolder::isValid() const {
        return _valid;
    }

    VKQueueHolder& VKQueueHolder::operator=(VKQueueHolder&& move) noexcept {
        _provider = move._provider;
        _queue = move._queue;
        _family = move._family;
        _index = move._index;
        _valid = move._valid;
        move._valid = false;
        return *this;
    }
}
