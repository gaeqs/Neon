//
// Created by gaeqs on 6/08/24.
//

#include "VKDevice.h"

#include <vector>
#include <stdexcept>
#include <neon/logging/Logger.h>

namespace neon::vulkan {
    namespace {
        const std::vector<const char*> DEVICE_EXTENSIONS = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_MAINTENANCE_4_EXTENSION_NAME
            //VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME
        };
    }

    VKDevice::VKDevice(VkDevice raw,
                       const VKQueueFamilyCollection& families,
                       const std::vector<uint32_t>& presentQueues)
        : _raw(raw),
          _queueProvider(std::make_unique<VKQueueProvider>(
              raw, families, presentQueues)),
          _external(true) {}

    VKDevice::VKDevice(VkPhysicalDevice physicalDevice,
                       VKPhysicalDeviceFeatures& features,
                       const VKQueueFamilyCollection& families)
        : _external(false) {
        std::vector<float> priorities;
        uint32_t maxQueues = 0;
        for (auto family: families.getFamilies()) {
            maxQueues = std::max(family.getCount(), maxQueues);
        }
        priorities.resize(maxQueues, 1.0f);


        std::vector<uint32_t> presentQueues;
        presentQueues.resize(families.getFamilies().size());

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve(families.getFamilies().size());

        for (auto family: families.getFamilies()) {
            VkDeviceQueueCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            info.queueFamilyIndex = family.getIndex();
            info.queueCount = family.getCount();
            info.pQueuePriorities = priorities.data();
            queueCreateInfos.push_back(info);
            presentQueues[family.getIndex()] = family.getCount();
        }


        const VkPhysicalDeviceFeatures2& f = features.getFeatures();
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = nullptr;
        createInfo.enabledExtensionCount = DEVICE_EXTENSIONS.size();
        createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = &f;

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &_raw) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }

        _queueProvider = std::make_unique<VKQueueProvider>(
            _raw,
            families,
            presentQueues
        );
    }

    VKDevice::~VKDevice() {
        if (!_external) {
            vkDestroyDevice(_raw, nullptr);
        }
    }

    VkDevice VKDevice::getRaw() const {
        return _raw;
    }

    VKQueueProvider* VKDevice::getQueueProvider() const {
        return _queueProvider.get();
    }
}
