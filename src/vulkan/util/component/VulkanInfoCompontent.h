//
// Created by gaeqs on 18/09/2024.
//

#ifndef VULKANINFOCOMPONTENT_H
#define VULKANINFOCOMPONTENT_H

#include <imgui.h>
#include <neon/structure/Component.h>

#include <vulkan/vulkan.h>
#include <vulkan/device/VKPhysicalDeviceFeatures.h>

namespace neon::vulkan {
    class VulkanInfoCompontent : public Component {
        VkPhysicalDeviceProperties _properties;
        std::vector<VkExtensionProperties> _extensions;
        const VKPhysicalDeviceFeatures* _features;

        static void tooltip(const char* desc);

        static void printProperty(const char* title,
                                  VkPhysicalDeviceType value);

        static void printProperty(const char* title, uint32_t value);

        static void printPropertyHex(const char* title, uint32_t value);

        static void printProperty(const char* title, int32_t value);

        static void printProperty(const char* title, uint64_t value);

        static void printProperty(const char* title, bool value);

        static void printPropertyBool(const char* title, VkBool32 value);

        static void printProperty(const char* title, float value);

        static void printProperty(const char* title, const char* value);

        static void printProperty(const char* title, const std::string& value);

        void physicalDeviceProperties() const;

        void physicalDeviceLimits() const;

        void physicalDeviceExtensions() const;

        void physicalDeviceFeatures() const;

    public:
        void onStart() override;

        void onPreDraw() override;
    };
}


#endif //VULKANINFOCOMPONTENT_H
