//
// Created by gaeqs on 4/08/24.
//

#include "VKQueueFamily.h"

namespace neon::vulkan {
    VKQueueFamily::Capabilities::Capabilities(bool graphics,
                                              bool compute,
                                              bool transfer,
                                              bool sparse_binding,
                                              bool protected_memory,
                                              bool video_decode,
                                              bool video_encode,
                                              bool optical_flow,
                                              bool present)
        : graphics(graphics),
          compute(compute),
          transfer(transfer),
          sparseBinding(sparse_binding),
          protectedMemory(protected_memory),
          videoDecode(video_decode),
          videoEncode(video_encode),
          opticalFlow(optical_flow),
          present(present) {
    }

    VKQueueFamily::Capabilities::Capabilities(VkQueueFlags flags)
        : graphics((flags & VK_QUEUE_GRAPHICS_BIT) > 0),
          compute((flags & VK_QUEUE_COMPUTE_BIT) > 0),
          transfer((flags & VK_QUEUE_TRANSFER_BIT) > 0),
          sparseBinding((flags & VK_QUEUE_SPARSE_BINDING_BIT) > 0),
          protectedMemory((flags & VK_QUEUE_PROTECTED_BIT) > 0),
          videoDecode((flags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) > 0),
          videoEncode((flags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR) > 0),
          opticalFlow((flags & VK_QUEUE_OPTICAL_FLOW_BIT_NV) > 0),
          present(false) {
    }

    bool VKQueueFamily::Capabilities::isCompatible(
        const Capabilities& requirements) const {
        if (requirements.graphics && !graphics) return false;
        if (requirements.compute && !compute) return false;
        if (requirements.transfer && !transfer) return false;
        if (requirements.sparseBinding && !sparseBinding) return false;
        if (requirements.protectedMemory && !protectedMemory) return false;
        if (requirements.videoDecode && !videoDecode) return false;
        if (requirements.videoEncode && !videoEncode) return false;
        if (requirements.opticalFlow && !opticalFlow) return false;
        if (requirements.present && !present) return false;
        return true;
    }

    VKQueueFamily::Capabilities VKQueueFamily::Capabilities::operator|(
        const Capabilities& other) const {
        return Capabilities(
            graphics || other.graphics,
            compute || other.compute,
            transfer || other.transfer,
            sparseBinding || other.sparseBinding,
            protectedMemory || other.protectedMemory,
            videoDecode || other.videoDecode,
            videoEncode || other.videoEncode,
            opticalFlow || other.opticalFlow,
            present || other.present
        );
    }

    VKQueueFamily::Capabilities VKQueueFamily::Capabilities::operator&(
        const Capabilities& other) const {
        return Capabilities(
            graphics && other.graphics,
            compute && other.compute,
            transfer && other.transfer,
            sparseBinding && other.sparseBinding,
            protectedMemory && other.protectedMemory,
            videoDecode && other.videoDecode,
            videoEncode && other.videoEncode,
            opticalFlow && other.opticalFlow,
            present && other.present
        );
    }

    VKQueueFamily::Capabilities VKQueueFamily::Capabilities::withGraphics() {
        return Capabilities(true);
    }

    VKQueueFamily::Capabilities VKQueueFamily::Capabilities::withPresent() {
        Capabilities c;
        c.present = true;
        return c;
    }


    VKQueueFamily::VKQueueFamily(VkPhysicalDevice device,
                                 VkSurfaceKHR surface,
                                 uint32_t index,
                                 const VkQueueFamilyProperties& properties)
        : _index(index),
          _count(properties.queueCount),
          _capabilities(properties.queueFlags),
          _timestamp(properties.timestampValidBits),
          _minImageGranularity(
              properties.minImageTransferGranularity.width,
              properties.minImageTransferGranularity.height,
              properties.minImageTransferGranularity.depth
          ) {
        VkBool32 presentSupport = false;

        if (surface != VK_NULL_HANDLE) {
            vkGetPhysicalDeviceSurfaceSupportKHR(
                device,
                index,
                surface,
                &presentSupport
            );
        }

        _capabilities.present = presentSupport != 0;
    }

    uint32_t VKQueueFamily::getIndex() const {
        return _index;
    }

    uint32_t VKQueueFamily::getCount() const {
        return _count;
    }

    const VKQueueFamily::Capabilities&
    VKQueueFamily::getCapabilities() const {
        return _capabilities;
    }

    uint32_t VKQueueFamily::getTimestamp() const {
        return _timestamp;
    }

    rush::Vec3f VKQueueFamily::getMinImageGranularity() const {
        return _minImageGranularity;
    }
}
