//
// Created by gaelr on 10/11/2022.
//

#ifndef NEON_VKAPPLICATION_H
#define NEON_VKAPPLICATION_H

#include <vector>
#include <memory>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <engine/render/CommandBuffer.h>
#include <engine/Application.h>

#include <vulkan/vulkan.h>
#include <vulkan/AbstractVKApplication.h>
#include <vulkan/VKSwapChainSupportDetails.h>
#include <vulkan/VKQueueFamilyIndices.h>

#include <util/profile/Profiler.h>

namespace neon {
    class Room;
}

namespace neon::vulkan {
    class VKApplication : public AbstractVKApplication {
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        const std::vector<const char*> VALIDATION_LAYERS = {
            "VK_LAYER_KHRONOS_validation"
        };

#ifdef NDEBUG
        static constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
        static constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif

        const std::vector<const char*> DEVICE_EXTENSIONS = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            //VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME
        };

        Application* _application;

        GLFWwindow* _window;
        std::string _name;
        float _width;
        float _height;

        VkInstance _instance;
        VkDebugUtilsMessengerEXT _debugMessenger;
        VkPhysicalDevice _physicalDevice;
        VKQueueFamilyIndices _familyIndices;
        VkDevice _device;
        VkQueue _graphicsQueue;
        VkQueue _presentQueue;
        VkSurfaceKHR _surface;
        VkSurfaceFormatKHR _surfaceFormat;

        VkSwapchainKHR _swapChain;
        VkFormat _swapChainImageFormat;
        VkExtent2D _swapChainExtent;
        uint32_t _swapChainCount;

        VkFormat _depthImageFormat;

        std::unique_ptr<CommandPool> _commandPool;

        std::vector<std::unique_ptr<CommandBuffer>> _commandBuffers;
        bool _recording;

        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;

        uint32_t _currentFrame;
        uint32_t _imageIndex;

        VkDescriptorPool _imGuiPool;

        FrameInformation _currentFrameInformation;

        // region VULKAN INITIALIZATION

        void createInstance();

        bool checkValidationLayerSupport();

        std::vector<const char*> getRequiredExtensions();

        void setupDebugMessenger();

        void createSurface();

        void pickPhysicalDevice();

        bool isDeviceSuitable(VkPhysicalDevice device, bool onlyDiscrete);

        VKQueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        void createLogicalDevice();

        void createSwapChain();

        VKSwapChainSupportDetails
        querySwapChainSupport(VkPhysicalDevice device);

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats);

        VkPresentModeKHR
        chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& availableModes);

        VkExtent2D
        chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        void createCommandPool();

        void createCommandBuffers();

        void createSyncObjects();

        void initImGui();

        void recreateSwapChain();

        void cleanupSwapChain();

        // endregion

    public:
        VKApplication(const VKApplication& other) = delete;

        VKApplication(std::string name, float width, float height);

        ~VKApplication() override;

        void init(neon::Application* application) override;

        [[nodiscard]] rush::Vec2i getWindowSize() const override;

        [[nodiscard]] FrameInformation
        getCurrentFrameInformation() const override;

        [[nodiscard]] CommandBuffer* getCurrentCommandBuffer() const override;

        void lockMouse(bool lock) override;

        Result<uint32_t, std::string> startGameLoop() override;

        void renderFrame(neon::Room* room) override;

        void preWindowCreation();

        void postWindowCreation();

        bool preUpdate(Profiler& profiler);

        void endDraw(Profiler& pofiler);

        void finishLoop();

        void internalForceSizeValues(int32_t width, int32_t height);

        [[nodiscard]] Application* getApplication() const;

        [[nodiscard]] uint32_t getCurrentFrame() const override;

        [[nodiscard]] uint32_t getCurrentSwapChainImage() const override;

        [[nodiscard]] uint32_t getMaxFramesInFlight() const override;

        [[nodiscard]] VkInstance getInstance() const override;

        [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const override;

        [[nodiscard]] VkDevice getDevice() const override;

        [[nodiscard]] VKQueueFamilyIndices getFamilyIndices() const override;

        [[nodiscard]] VkQueue getGraphicsQueue() const override;

        [[nodiscard]] VkQueue getPresentQueue() const;

        [[nodiscard]] VkSurfaceKHR getSurface() const;

        [[nodiscard]] VkSwapchainKHR getSwapChain() const;

        [[nodiscard]] VkFormat getSwapChainImageFormat() const override;

        [[nodiscard]] VkFormat getDepthImageFormat() const override;

        [[nodiscard]] VkExtent2D getSwapChainExtent() const override;

        [[nodiscard]] uint32_t getSwapChainCount() const override;

        [[nodiscard]] CommandPool* getCommandPool() const override;

        [[nodiscard]] VkDescriptorPool getImGuiPool() const override;

        [[nodiscard]] bool isRecordingCommandBuffer() const override;
    };
}

#endif //NEON_VKAPPLICATION_H
