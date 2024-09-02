//
// Created by gaeqs on 4/08/24.
//

#ifndef VKQUEUEFAMILY_H
#define VKQUEUEFAMILY_H


#include <cstdint>
#include <vulkan/vulkan.h>
#include <rush/rush.h>

namespace neon::vulkan {
    class VKQueueFamily {
    public:
        struct Capabilities {
            bool graphics;
            bool compute;
            bool transfer;
            bool sparseBinding;
            bool protectedMemory;
            bool videoDecode;
            bool videoEncode;
            bool opticalFlow;
            bool present;

            explicit Capabilities(bool graphics = false,
                                  bool compute = false,
                                  bool transfer = false,
                                  bool sparse_binding = false,
                                  bool protected_memory = false,
                                  bool video_decode = false,
                                  bool video_encode = false,
                                  bool optical_flow = false,
                                  bool present = false);

            explicit Capabilities(VkQueueFlags flags);

            [[nodiscard]] bool isCompatible(
                const Capabilities& requirements) const;

            Capabilities operator|(const Capabilities& other) const;

            Capabilities operator&(const Capabilities& other) const;

            static Capabilities withGraphics();

            static Capabilities withPresent();
        };

    private:
        uint32_t _index;
        uint32_t _count;
        Capabilities _capabilities;
        uint32_t _timestamp;
        rush::Vec3f _minImageGranularity;

    public:
        VKQueueFamily(VkPhysicalDevice device,
                      VkSurfaceKHR surface,
                      uint32_t index,
                      const VkQueueFamilyProperties& properties);

        [[nodiscard]] uint32_t getIndex() const;

        [[nodiscard]] uint32_t getCount() const;

        [[nodiscard]] const Capabilities& getCapabilities() const;

        [[nodiscard]] uint32_t getTimestamp() const;

        [[nodiscard]] rush::Vec3f getMinImageGranularity() const;
    };
}

#endif //VKQUEUEFAMILY_H
