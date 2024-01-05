//
// Created by gaelr on 10/11/2022.
//

#include "VKApplication.h"

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <utility>

#include <engine/structure/Room.h>
#include <vulkan/util/VKUtil.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <implot.h>

namespace neon::vulkan {
    void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        auto* application = static_cast<VKApplication*>(
            glfwGetWindowUserPointer(
                window));
        application->internalForceSizeValues(width, height);
    }

    void key_size_callback(GLFWwindow* window, int key, int scancode,
                           int action, int mods) {
        auto* application = static_cast<VKApplication*>(
            glfwGetWindowUserPointer(
                window));

        application->getApplication()
                ->invokeKeyEvent(key, scancode, action, mods);
    }

    void mouse_button_callback(GLFWwindow* window, int button,
                               int action, int mods) {
        auto* application = static_cast<VKApplication*>(
            glfwGetWindowUserPointer(
                window));

        application->getApplication()
                ->invokeMouseButtonEvent(button, action, mods);
    }

    void cursor_pos_callback(GLFWwindow* window, double x, double y) {
        auto* application = static_cast<VKApplication*>(
            glfwGetWindowUserPointer(window));
        application->getApplication()->invokeCursorPosEvent(x, y);
    }

    void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
        auto* application = static_cast<VKApplication*>(
            glfwGetWindowUserPointer(window));
        application->getApplication()->invokeScrollEvent(xOffset, yOffset);
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {
        if (messageSeverity >=
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            std::cerr << "[VULKAN ERROR]: " << pCallbackData->pMessage
                    << std::endl;
            std::cerr << "------------------------------" << std::endl;
        }
        else if (messageSeverity >=
                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            std::cerr << "[VULKAN WARNING]: " << pCallbackData->pMessage
                    << std::endl;
            std::cerr << "------------------------------" << std::endl;
        }
        else {
            std::cout << "[VULKAN DEBUG]: " << pCallbackData->pMessage
                    << std::endl;
            std::cout << "------------------------------" << std::endl;
        }

        return VK_FALSE;
    }

    static VkResult createDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger
    ) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void destroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator
    ) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    VKApplication::VKApplication(std::string name, float width,
                                 float height) : _application(nullptr),
                                                 _window(nullptr),
                                                 _name(std::move(name)),
                                                 _width(width),
                                                 _height(height),
                                                 _instance(VK_NULL_HANDLE),
                                                 _debugMessenger(
                                                     VK_NULL_HANDLE),
                                                 _physicalDevice(
                                                     VK_NULL_HANDLE),
                                                 _device(VK_NULL_HANDLE),
                                                 _graphicsQueue(VK_NULL_HANDLE),
                                                 _presentQueue(VK_NULL_HANDLE),
                                                 _surface(VK_NULL_HANDLE),
                                                 _surfaceFormat(),
                                                 _swapChain(VK_NULL_HANDLE),
                                                 _swapChainImageFormat(),
                                                 _swapChainExtent(),
                                                 _swapChainCount(0),
                                                 _depthImageFormat(),
                                                 _commandPool(nullptr),
                                                 _commandBuffers(),
                                                 _recording(false),
                                                 _imageAvailableSemaphores(),
                                                 _renderFinishedSemaphores(),
                                                 _currentFrame(0),
                                                 _imageIndex(0),
                                                 _imGuiPool(VK_NULL_HANDLE),
                                                 _currentFrameInformation() {
    }

    void VKApplication::init(neon::Application* application) {
        _application = application;
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        preWindowCreation();

        _window = glfwCreateWindow(static_cast<int>(_width),
                                   static_cast<int>(_height),
                                   _name.c_str(), nullptr, nullptr);
        if (!_window) {
            throw std::runtime_error("Failed to open GLFW window");
        }

        glfwSetWindowUserPointer(_window, this);
        glfwSetWindowSizeCallback(_window, framebuffer_size_callback);
        glfwSetKeyCallback(_window, key_size_callback);
        glfwSetMouseButtonCallback(_window, mouse_button_callback);
        glfwSetCursorPosCallback(_window, cursor_pos_callback);
        glfwSetScrollCallback(_window, scroll_callback);

        postWindowCreation();
    }

    rush::Vec2i VKApplication::getWindowSize() const {
        return {_width, _height};
    }

    FrameInformation VKApplication::getCurrentFrameInformation() const {
        return _currentFrameInformation;
    }

    CommandBuffer* VKApplication::getCurrentCommandBuffer() const {
        return _commandBuffers.empty()
                   ? nullptr
                   : _commandBuffers[_currentFrame].get();
    }

    void VKApplication::lockMouse(bool lock) {
        if (lock) {
            glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    Result<uint32_t, std::string> VKApplication::startGameLoop() {
        if (_window == nullptr) {
            return {"Window is not initialized"};
        }

        uint32_t frames = 0;

        auto lastTick = std::chrono::high_resolution_clock::now();
        float lastFrameProcessTime = 0.0f;
        try {
            while (!glfwWindowShouldClose(_window)) {
                DEBUG_PROFILE(_application->getProfiler(), tick);
                auto now = std::chrono::high_resolution_clock::now();
                auto duration = now - lastTick;
                lastTick = now;

                float seconds = static_cast<float>(duration.count()) * 1e-9f;

                _currentFrameInformation = {
                    frames,
                    seconds,
                    lastFrameProcessTime
                };

                renderFrame(_application->getRoom().get());

                now = std::chrono::high_resolution_clock::now();
                auto processTime = now - lastTick;

                lastFrameProcessTime =
                        static_cast<float>(processTime.count()) * 1e-9f;

                ++frames;
            }
            finishLoop();
        }
        catch (const std::exception& exception) {
            return {exception.what()};
        }

        return {frames};
    }

    void VKApplication::renderFrame(neon::Room* room) {
        bool preUpdateDone; {
            DEBUG_PROFILE_ID(_application->getProfiler(), preUpdate,
                             "preUpdate (GPU Wait)");
            preUpdateDone = preUpdate(_application->getProfiler());
        }

        if (preUpdateDone) {
            glfwPollEvents();

            if (room != nullptr) {
                room->update(_currentFrameInformation.currentDeltaTime);
                room->preDraw();
                room->draw();
            } {
                DEBUG_PROFILE(_application->getProfiler(), endDraw);
                endDraw(_application->getProfiler());
            }
        }
    }

    void VKApplication::preWindowCreation() {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    void VKApplication::postWindowCreation() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createCommandPool();
        createCommandBuffers();
        createSyncObjects();
        initImGui();
    }

    bool VKApplication::preUpdate(Profiler& profiler) { {
            DEBUG_PROFILE_ID(profiler, wait, "Wait for GPU");
            _commandBuffers[_currentFrame]->reset();
        }

        auto& render = _application->getRender(); {
            DEBUG_PROFILE_ID(profiler, recreation, "FB Recreation");
            render->checkFrameBufferRecreationConditions();
        }

        VkResult result; {
            DEBUG_PROFILE_ID(profiler, adquireImage, "Image Acquisition");
            result = vkAcquireNextImageKHR(
                _device, _swapChain, UINT64_MAX,
                _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE,
                &_imageIndex);
        }

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            {
                DEBUG_PROFILE_ID(profiler, recreation2, "SC/FB Recreation");
                recreateSwapChain();
                render->checkFrameBufferRecreationConditions();
            }
            return false;
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to acquire swap chain image!");
        } {
            DEBUG_PROFILE_ID(profiler, beginCB, "Begin Command Buffer");
            _commandBuffers[_currentFrame]->begin(false);
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        _recording = true;

        return true;
    }

    void VKApplication::endDraw(Profiler& profiler) {
        _commandBuffers[_currentFrame]->end();

        _recording = false;

        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };


        _commandBuffers[_currentFrame]->getImplementation().submit(
            1, &_imageAvailableSemaphores[_currentFrame], waitStages,
            1, &_renderFinishedSemaphores[_currentFrame]);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &_renderFinishedSemaphores[_currentFrame];

        VkSwapchainKHR swapChains[] = {_swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &_imageIndex;
        presentInfo.pResults = nullptr; {
            DEBUG_PROFILE_ID(profiler, present, "Present");
            vkQueuePresentKHR(_presentQueue, &presentInfo);
        }

        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }


    void VKApplication::finishLoop() {
        vkDeviceWaitIdle(_device);
    }

    void VKApplication::internalForceSizeValues(int32_t width, int32_t height) {
        _width = width;
        _height = height;
    }

    void VKApplication::createInstance() {
        if (ENABLE_VALIDATION_LAYERS && !checkValidationLayerSupport()) {
            throw std::runtime_error(
                "Validation layer required but not available!");
        }

        VkApplicationInfo applicationInfo{};
        VkInstanceCreateInfo createInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "My Application";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_2;

        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        createInfo.enabledLayerCount = 0;

        if (ENABLE_VALIDATION_LAYERS) {
            createInfo.enabledLayerCount = VALIDATION_LAYERS.size();
            createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create instance!");
        }

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                               nullptr);
        std::vector<VkExtensionProperties> instanceExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                               instanceExtensions.data());
        for (const auto& item: instanceExtensions) {
            std::cout << item.extensionName << std::endl;
        }
    }

    bool VKApplication::checkValidationLayerSupport() {
        uint32_t count;
        vkEnumerateInstanceLayerProperties(&count, nullptr);
        std::vector<VkLayerProperties> layers(count);
        vkEnumerateInstanceLayerProperties(&count, layers.data());
        for (const auto& layer: VALIDATION_LAYERS) {
            bool found = false;
            for (const auto& property: layers) {
                if (strcmp(layer, property.layerName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        return true;
    }

    std::vector<const char*> VKApplication::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions,
                                            glfwExtensions +
                                            glfwExtensionCount);

        if (ENABLE_VALIDATION_LAYERS) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void VKApplication::setupDebugMessenger() {
        if (!ENABLE_VALIDATION_LAYERS) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType =
                VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional

        if (createDebugUtilsMessengerEXT(_instance, &createInfo, nullptr,
                                         &_debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("Failed t set up debug messenger!");
        }
    }

    void VKApplication::createSurface() {
        if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void VKApplication::pickPhysicalDevice() {
        _physicalDevice = VK_NULL_HANDLE;
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error(
                "Failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

        for (const auto& device: devices) {
            if (isDeviceSuitable(device, true)) {
                _physicalDevice = device;
                break;
            }
        }

        if (_physicalDevice == VK_NULL_HANDLE) {
            for (const auto& device: devices) {
                if (isDeviceSuitable(device, false)) {
                    _physicalDevice = device;
                    break;
                }
            }
        }

        if (_physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error(
                "Failed to find GPUs with Vulkan support!");
        }
        else {
            std::cout << "Selected physical device: ";
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(_physicalDevice, &deviceProperties);
            std::cout << deviceProperties.deviceName << std::endl;
        }
    }

    bool
    VKApplication::isDeviceSuitable(VkPhysicalDevice device,
                                    bool onlyDiscrete) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        auto families = findQueueFamilies(device);

        // extensions
        uint32_t extensionCount;

        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                             nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                             extensions.data());

        auto requiredExtensions = std::unordered_set<std::string>(
            DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());
        for (const auto& item: extensions) {
            requiredExtensions.erase(item.extensionName);
        }

        VKSwapChainSupportDetails swapChainSupport = querySwapChainSupport(
            device);
        bool swapChainAdequate = !swapChainSupport.formats.empty() &&
                                 !swapChainSupport.presentModes.empty();

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return (!onlyDiscrete || deviceProperties.deviceType ==
                VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) &&
               families.isComplete() && requiredExtensions.empty() &&
               swapChainAdequate && supportedFeatures.samplerAnisotropy
               && supportedFeatures.geometryShader
               && supportedFeatures.wideLines;
    }

    VKQueueFamilyIndices
    VKApplication::findQueueFamilies(VkPhysicalDevice device) {
        VKQueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                                 nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                                 queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily: queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphics = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface,
                                                 &presentSupport);

            if (presentSupport) {
                indices.present = i;
            }

            i++;
        }

        return indices;
    }

    void VKApplication::createLogicalDevice() {
        _familyIndices = findQueueFamilies(_physicalDevice);

        float priority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        for (const auto& item: _familyIndices.getValidIndices()) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = item;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &priority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.geometryShader = VK_TRUE;
        deviceFeatures.wideLines = VK_TRUE;

        VkPhysicalDeviceVulkan12Features vulkan12Features{};
        vulkan12Features.sType =
                VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        vulkan12Features.separateDepthStencilLayouts = VK_TRUE;


        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = DEVICE_EXTENSIONS.size();
        createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = &vulkan12Features;
        if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }

        vkGetDeviceQueue(
            _device,
            _familyIndices.graphics.value(),
            0,
            &_graphicsQueue
        );
        vkGetDeviceQueue(
            _device,
            _familyIndices.present.value(),
            0,
            &_presentQueue
        );
    }

    void VKApplication::createSwapChain() {
        auto support = querySwapChainSupport(_physicalDevice);
        auto format = chooseSwapSurfaceFormat(support.formats);
        auto presentMode = chooseSwapPresentMode(support.presentModes);
        auto extent = chooseSwapExtent(support.capabilities);

        uint32_t imageCount = support.capabilities.minImageCount + 1;

        if (support.capabilities.maxImageCount > 0 &&
            imageCount > support.capabilities.maxImageCount) {
            imageCount = support.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = format.format;
        createInfo.imageColorSpace = format.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = {
            _familyIndices.graphics.value(),
            _familyIndices.present.value()
        };
        if (_familyIndices.graphics != _familyIndices.present) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = support.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create swap chain!");
        }

        _surfaceFormat = format;
        _swapChainImageFormat = format.format;
        _swapChainExtent = extent;

        auto depthFormat = vulkan_util::findSupportedFormat(
            _physicalDevice,
            {
                VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D24_UNORM_S8_UINT
            },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
        if (!depthFormat.has_value()) {
            throw std::runtime_error("Couldn't find depth format!");
        }

        _depthImageFormat = depthFormat.value();
        ++_swapChainCount;
    }

    VKSwapChainSupportDetails
    VKApplication::querySwapChainSupport(VkPhysicalDevice device) {
        VKSwapChainSupportDetails details{};
        uint32_t formatCount;
        uint32_t presentModeCount;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface,
                                                  &details.capabilities);

        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount,
                                             nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount,
                                                 details.formats.data());
        }

        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface,
                                                  &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface,
                &presentModeCount,
                details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR VKApplication::chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& format: availableFormats) {
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM) {
                return format;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR VKApplication::chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR>& availableModes) {
        for (const auto& mode: availableModes) {
            if (mode == VK_PRESENT_MODE_FIFO_KHR) {
                return mode;
            }
        }

        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    VkExtent2D
    VKApplication::chooseSwapExtent(
        const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width !=
            std::numeric_limits<uint32_t>::max()) {
            // Fixed mode
            return capabilities.currentExtent;
        }
        else {
            // Bounds mode
            int width, height;
            glfwGetFramebufferSize(_window, &width, &height);
            return {
                std::clamp(static_cast<uint32_t>(width),
                           capabilities.minImageExtent.width,
                           capabilities.maxImageExtent.width),
                std::clamp(static_cast<uint32_t>(height),
                           capabilities.minImageExtent.height,
                           capabilities.maxImageExtent.height),
            };
        }
    }

    void VKApplication::createCommandPool() {
        _commandPool = std::make_unique<CommandPool>(_application);
    }

    void VKApplication::createCommandBuffers() {
        _commandBuffers.clear();
        _commandBuffers.reserve(MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            _commandBuffers.emplace_back(
                std::make_unique<CommandBuffer>(_application, true));
        }
    }

    void VKApplication::createSyncObjects() {
        _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr,
                                  &_imageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateSemaphore(_device, &semaphoreInfo, nullptr,
                                  &_renderFinishedSemaphores[i]) !=
                VK_SUCCESS) {
                throw std::runtime_error(
                    "Failed to create synchronization objects!");
            }
        }
    }

    void VKApplication::initImGui() {
        VkDescriptorPoolSize pool_sizes[] = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
        };

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        if (vkCreateDescriptorPool(_device, &pool_info, nullptr, &_imGuiPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to init ImGui!");
        }

        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(_window, true);

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    void VKApplication::recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(_window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(_window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(_device);
        cleanupSwapChain();
        createSwapChain();
    }

    void VKApplication::cleanupSwapChain() {
        vkDestroySwapchainKHR(_device, _swapChain, nullptr);
    }

    VKApplication::~VKApplication() {
        vkDestroyDescriptorPool(_device, _imGuiPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImPlot::DestroyContext();
        ImGui::DestroyContext();

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
        }

        _commandBuffers.clear();
        _commandPool = nullptr;

        cleanupSwapChain();
        vkDestroyDevice(_device, nullptr);
        vkDestroySurfaceKHR(_instance, _surface, nullptr);

        if (ENABLE_VALIDATION_LAYERS) {
            destroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
        }

        vkDestroyInstance(_instance, nullptr);

        glfwTerminate();
    }

    uint32_t VKApplication::getCurrentFrame() const {
        return _currentFrame;
    }

    uint32_t VKApplication::getCurrentSwapChainImage() const {
        return _imageIndex;
    }

    uint32_t VKApplication::getMaxFramesInFlight() const {
        return MAX_FRAMES_IN_FLIGHT;
    }

    VkInstance VKApplication::getInstance() const {
        return _instance;
    }

    VkPhysicalDevice VKApplication::getPhysicalDevice() const {
        return _physicalDevice;
    }

    VkDevice VKApplication::getDevice() const {
        return _device;
    }

    VKQueueFamilyIndices VKApplication::getFamilyIndices() const {
        return _familyIndices;
    }

    VkQueue VKApplication::getGraphicsQueue() const {
        return _graphicsQueue;
    }

    VkQueue VKApplication::getPresentQueue() const {
        return _presentQueue;
    }

    VkSurfaceKHR VKApplication::getSurface() const {
        return _surface;
    }

    VkSwapchainKHR VKApplication::getSwapChain() const {
        return _swapChain;
    }

    CommandPool* VKApplication::getCommandPool() const {
        return _commandPool.get();
    }

    VkDescriptorPool VKApplication::getImGuiPool() const {
        return _imGuiPool;
    }

    bool VKApplication::isRecordingCommandBuffer() const {
        return _recording;
    }

    VkFormat VKApplication::getSwapChainImageFormat() const {
        return _swapChainImageFormat;
    }

    VkFormat VKApplication::getDepthImageFormat() const {
        return _depthImageFormat;
    }

    VkExtent2D VKApplication::getSwapChainExtent() const {
        return _swapChainExtent;
    }

    uint32_t VKApplication::getSwapChainCount() const {
        return _swapChainCount;
    }

    Application* VKApplication::getApplication() const {
        return _application;
    }
}
