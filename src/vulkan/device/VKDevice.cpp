//
// Created by gaeqs on 6/08/24.
//

#include "VKDevice.h"

#include <vector>
#include <stdexcept>

namespace neon::vulkan
{
    VKDevice::VKDevice(VkDevice raw, const VKQueueFamilyCollection& families, const VKPhysicalDeviceFeatures& features,
                       const std::vector<uint32_t>& presentQueues) :
        _raw(raw),
        _queueProvider(std::make_unique<VKQueueProvider>(raw, families, presentQueues)),
        _enabledFeatures(features),
        _external(true)
    {
    }

    VKDevice::VKDevice(VkPhysicalDevice physicalDevice, const VKPhysicalDeviceFeatures& features,
                       const VKQueueFamilyCollection& families) :
        _enabledFeatures(features),
        _external(false)
    {
        std::vector<float> priorities;
        uint32_t maxQueues = 0;
        for (auto family : families.getFamilies()) {
            maxQueues = std::max(family.getCount(), maxQueues);
        }
        priorities.resize(maxQueues, 1.0f);

        std::vector<uint32_t> presentQueues;
        presentQueues.resize(families.getFamilies().size());

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve(families.getFamilies().size());

        for (auto family : families.getFamilies()) {
            VkDeviceQueueCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.queueFamilyIndex = family.getIndex();
            info.queueCount = family.getCount();
            info.pQueuePriorities = priorities.data();
            queueCreateInfos.push_back(info);
            presentQueues[family.getIndex()] = family.getCount();
        }

        std::vector<const char*> rawExtensions;
        rawExtensions.reserve(_enabledFeatures.extensions.size());

        for (const auto& extension : _enabledFeatures.extensions) {
            rawExtensions.emplace_back(extension.c_str());
        }

        VkPhysicalDeviceFeatures2 f = _enabledFeatures.toFeatures2();
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = nullptr;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(rawExtensions.size());
        createInfo.ppEnabledExtensionNames = rawExtensions.data();
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = &f;

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &_raw) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }

        _queueProvider = std::make_unique<VKQueueProvider>(_raw, families, presentQueues);
    }

    VKDevice::~VKDevice()
    {
        if (!_external) {
            vkDestroyDevice(_raw, nullptr);
        }
    }

    VkDevice VKDevice::getRaw() const
    {
        return _raw;
    }

    VKQueueProvider* VKDevice::getQueueProvider() const
    {
        return _queueProvider.get();
    }

    const VKPhysicalDeviceFeatures& VKDevice::getEnabledFeatures() const
    {
        return _enabledFeatures;
    }
} // namespace neon::vulkan
