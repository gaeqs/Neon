//
// Created by gaeqs on 23/09/2024.
//

#ifndef VKAPPLICATIONCREATEINFO_H
#define VKAPPLICATIONCREATEINFO_H

#include <vector>

#include <neon/structure/ApplicationCreateInfo.h>
#include <vulkan/device/VKPhysicalDevice.h>

namespace neon::vulkan
{
    enum class InclusionMode
    {
        EXCLUDE_ALL,
        INCLUDE_ALL
    };

    /**
     * Information required to create al vulkan application.
     */
    struct VKApplicationCreateInfo : ApplicationCreateInfo
    {
#ifdef NDEBUG
        /**
         * Whether vulkan validation layers should be enabled.
         */
        bool enableValidationLayers = false;
#else
        /**
         * Whether vulkan validation layers should be enabled.
         */
        bool enableValidationLayers = true;
#endif

        /**
         * Enables ImGui multi-viewport mode.
         * This mode is kind of broken in X11 systems, and it doesn't work on wayland.
         */
        bool imGuiEnableMultiViewport = false;

        /**
         * Enable decorators for external viewports.
         * This mode is even more broken than the multi viewport system.
         * Use with caution!
         */
        bool imGuiMultiViewportDecorators = false;

        /**
         * Whether all available extensions should be enabled
         * by default.
         */
        InclusionMode defaultExtensionInclusion = InclusionMode::EXCLUDE_ALL;

        /**
         * Whether all features included in "extraFeatures"
         * and in the default feature structures should be enabled
         * by default.
         *
         * The default feature structures are the basic features,
         * Vulkan11, Vulkan12 and Vulkan13.
         */
        InclusionMode defaultFeatureInclusion = InclusionMode::EXCLUDE_ALL;

        /**
         * The extra feature structures that should be included
         * in the physical devices.
         */
        std::vector<VKFeatureHolder> extraFeatures = {};

        /**
         * This function filters the physical devices
         * that can be used by the application.
         */
        std::function<bool(const VKPhysicalDevice&)> deviceFilter = defaultDeviceFilter;

        /**
         * This function is used to select the best physical device
         * for the application.
         *
         * The application will select the physical device with the
         * biggest returned number.
         */
        std::function<size_t(const VKPhysicalDevice&)> deviceSorter = defaultDeviceSorter;

        /**
         * This function is used to configure the features of the selected
         * physical device.
         */
        std::function<void(const VKPhysicalDevice&, VKPhysicalDeviceFeatures&)> featuresConfigurator =
            defaultFeaturesConfigurer;

        static bool defaultDeviceFilter(const VKPhysicalDevice& device)
        {
            bool hasGraphics = false;
            bool hasPresent = false;
            for (auto& family : device.getFamilyCollection().getFamilies()) {
                hasGraphics |= family.getCapabilities().graphics;
                hasPresent |= family.getCapabilities().present;
            }

            // Has support for graphics and presentation.
            if (!hasGraphics || !hasPresent) {
                return false;
            }

            // Has the required extensions.
            if (!device.getFeatures().hasExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
                return false;
            }

            if (!device.getFeatures().hasExtension(VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME)) {
                return false;
            }

            return true;
        }

        static size_t defaultDeviceSorter(const VKPhysicalDevice& device)
        {
            switch (device.getProperties().deviceType) {
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    return 3;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    return 2;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    return 1;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                default:
                    return 0;
            }
        }

        static void defaultFeaturesConfigurer(const VKPhysicalDevice& device, VKPhysicalDeviceFeatures& features)
        {
            features
                .findFeature<VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT>(
                    VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT)
                .value()
                ->swapchainMaintenance1 = true;
            features.extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            features.extensions.emplace_back(VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME);
        }
    };
} // namespace neon::vulkan

#endif // VKAPPLICATIONCREATEINFO_H
