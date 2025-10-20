//
// Created by gaeqs on 20/09/2024.
//

#ifndef VKPHYSICALDEVICEFEATURES_H
#define VKPHYSICALDEVICEFEATURES_H

#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace neon::vulkan
{
    struct VKDefaultExtension
    {
        VkStructureType sType;
        const void* pNext;
    };

    struct VKFeatureHolder
    {
        size_t size;
        char* raw;

        VKFeatureHolder(const char* data, size_t size);

        template<typename T>
        explicit VKFeatureHolder(const T& t) :
            VKFeatureHolder(reinterpret_cast<const char*>(&t), sizeof(T))
        {
        }

        VKFeatureHolder(const VKFeatureHolder& other);

        ~VKFeatureHolder();

        VKFeatureHolder& operator=(const VKFeatureHolder& other);

        [[nodiscard]] VKDefaultExtension* asDefaultExtensions();

        [[nodiscard]] const VKDefaultExtension* asDefaultExtensions() const;

        template<typename Feature>
        [[nodiscard]] Feature* asFeature()
        {
            return reinterpret_cast<Feature*>(raw);
        }

        template<typename Feature>
        [[nodiscard]] const Feature* asFeature() const
        {
            return reinterpret_cast<const Feature*>(raw);
        }
    };

    struct VKPhysicalDeviceFeatures
    {
        VkPhysicalDeviceFeatures basicFeatures = {};
        std::vector<VKFeatureHolder> features;
        std::vector<std::string> extensions;

        VKPhysicalDeviceFeatures(const VKPhysicalDeviceFeatures& other);

        VKPhysicalDeviceFeatures();

        explicit VKPhysicalDeviceFeatures(VkPhysicalDevice device,
                                          const std::vector<VKFeatureHolder>& extraFeatures = {});

        [[nodiscard]] bool hasExtension(const std::string& extension) const;

        [[nodiscard]] VkPhysicalDeviceFeatures2 toFeatures2() const;

        VKPhysicalDeviceFeatures& operator=(const VKPhysicalDeviceFeatures& o);

        template<typename Feature>
        std::optional<Feature*> findFeature(VkStructureType sType)
        {
            for (auto& feature : features) {
                if (feature.asDefaultExtensions()->sType == sType) {
                    return {feature.asFeature<Feature>()};
                }
            }
            return {};
        }

        template<typename Feature>
        std::optional<const Feature*> findFeature(VkStructureType sType) const
        {
            for (auto& feature : features) {
                if (feature.asDefaultExtensions()->sType == sType) {
                    return {feature.asFeature<Feature>()};
                }
            }
            return {};
        }

      private:
        void reweave();
    };
} // namespace neon::vulkan

#endif //VKPHYSICALDEVICEFEATURES_H
