//
// Created by gaeqs on 6/08/24.
//

#include "VKDevice.h"

#include <vector>
#include <stdexcept>

namespace neon::vulkan
{
    void VKDevice::createAllocator(VkInstance instance, VkPhysicalDevice physicalDevice)
    {
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        allocatorCreateInfo.physicalDevice = physicalDevice;
        allocatorCreateInfo.device = _raw;
        allocatorCreateInfo.instance = instance;
        allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

        vmaCreateAllocator(&allocatorCreateInfo, &_allocator);
    }

    VKDevice::VKDevice(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice raw,
                       const VKQueueFamilyCollection& families, const VKPhysicalDeviceFeatures& features,
                       const std::vector<uint32_t>& presentQueues) :
        _raw(raw),
        _queueProvider(std::make_unique<VKQueueProvider>(raw, families, presentQueues)),
        _enabledFeatures(features),
        _external(true)
    {
        createAllocator(instance, physicalDevice);
    }

    VKDevice::VKDevice(VkInstance instance, VkPhysicalDevice physicalDevice, const VKPhysicalDeviceFeatures& features,
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

        createAllocator(instance, physicalDevice);
    }

    VKDevice::~VKDevice()
    {
        vkDeviceWaitIdle(_raw);
        vmaDestroyAllocator(_allocator);
        if (!_external) {
            vkDestroyDevice(_raw, nullptr);
        }
    }

    VkDevice VKDevice::getRaw() const
    {
        return _raw;
    }

    VmaAllocator VKDevice::getAllocator() const
    {
        return _allocator;
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
