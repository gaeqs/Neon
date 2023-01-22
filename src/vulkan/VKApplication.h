//
// Created by gaelr on 10/11/2022.
//

#ifndef NEON_VKAPPLICATION_H
#define NEON_VKAPPLICATION_H

#define GLFW_INCLUDE_VULKAN

#include <vector>
#include <memory>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/VKSwapChainSupportDetails.h>
#include <vulkan/VKQueueFamilyIndices.h>

class Room;

class VKApplication {

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
            VK_KHR_SEPARATE_DEPTH_STENCIL_LAYOUTS_EXTENSION_NAME
    };

    GLFWwindow* _window;

    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debugMessenger;
    VkPhysicalDevice _physicalDevice;
    VkDevice _device;
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;
    VkSurfaceKHR _surface;
    VkSurfaceFormatKHR _surfaceFormat;

    VkSwapchainKHR _swapChain;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;

    VkFormat _depthImageFormat;

    bool _framebufferResized;

    VkCommandPool _commandPool;

    std::vector<VkCommandBuffer> _commandBuffers;
    bool _recording = false;

    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;

    uint32_t _currentFrame = 0;
    uint32_t _imageIndex;

    std::shared_ptr<Room> _room;

    VkDescriptorPool _imGuiPool;

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

    VKSwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR
    chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void createCommandPool();

    void createCommandBuffers();

    void createSyncObjects();

    void initImGui();

    void recreateSwapChain();

    void cleanupSwapChain();

    // endregion

public:

    VKApplication(const VKApplication& other) = delete;

    VKApplication() = default;

    ~VKApplication();

    void preWindowCreation();

    void postWindowCreation(GLFWwindow* window);

    bool preUpdate();

    void endDraw();

    void internalForceSizeValues(int32_t width, int32_t height);

    void internalKeyEvent(int key, int scancode, int action, int mods);

    void internalCursorPosEvent(double x, double y);

    [[nodiscard]] uint32_t getCurrentFrame() const;

    [[nodiscard]] uint32_t getCurrentSwapChainImage() const;

    [[nodiscard]] uint32_t getMaxFramesInFlight() const;

    [[nodiscard]] VkInstance getInstance() const;

    [[nodiscard]] VkPhysicalDevice getPhysicalDevice() const;

    [[nodiscard]] VkDevice getDevice() const;

    [[nodiscard]] VkQueue getGraphicsQueue() const;

    [[nodiscard]] VkQueue getPresentQueue() const;

    [[nodiscard]] VkSurfaceKHR getSurface() const;

    [[nodiscard]] VkSwapchainKHR getSwapChain() const;

    [[nodiscard]] VkFormat getSwapChainImageFormat() const;

    [[nodiscard]] VkFormat getDepthImageFormat() const;

    const VkExtent2D& getSwapChainExtent() const;

    [[nodiscard]] VkCommandPool getCommandPool() const;

    [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const;

    [[nodiscard]] VkDescriptorPool getImGuiPool() const;

    [[nodiscard]] bool isRecordingCommandBuffer() const;

    void setRoom(const std::shared_ptr<Room>& room);

};


#endif //NEON_VKAPPLICATION_H
