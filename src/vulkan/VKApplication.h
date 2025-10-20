//
// Created by gaelr on 10/11/2022.
//

#ifndef NEON_VKAPPLICATION_H
#define NEON_VKAPPLICATION_H

#include <vector>
#include <thread>

#define GLFW_INCLUDE_VULKAN


#include <GLFW/glfw3.h>

#include <neon/render/buffer/CommandBuffer.h>
#include <neon/structure/Application.h>
#include <neon/util/profile/Profiler.h>

#include <vulkan/vulkan.h>
#include <vulkan/sync/VKSemaphore.h>
#include <vulkan/AbstractVKApplication.h>
#include <vulkan/device/VKDevice.h>
#include <vulkan/VKApplicationCreateInfo.h>

namespace neon
{
    class Room;
}

namespace neon::vulkan
{
    class VKApplication : public AbstractVKApplication
    {
        static constexpr int MAX_FRAMES_IN_FLIGHT = 3;

        const std::vector<const char*> VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};

        VKApplicationCreateInfo _createInfo;

        Application* _application;

        GLFWwindow* _window;
        std::string _name;
        float _width;
        float _height;

        VkInstance _instance;
        VkDebugUtilsMessengerEXT _debugMessenger;
        VKPhysicalDevice _physicalDevice;

        VKDevice* _device;
        VKQueueHolder _graphicQueue;
        VKQueueHolder _presentQueue;

        VkSurfaceKHR _surface;
        VkSurfaceFormatKHR _surfaceFormat;

        VkSwapchainKHR _swapChain;
        VkFormat _swapChainImageFormat;
        VkExtent2D _swapChainExtent;
        uint32_t _swapChainCount;

        TextureFormat _depthImageFormat;
        VkFormat _vkDepthImageFormat;

        CommandPoolHolder _commandPool;
        VKResourceBin _bin;

        CommandBuffer* _currentCommandBuffer;
        bool _recording;

        uint32_t _swapChainImageCount;
        std::unordered_map<uint32_t, std::shared_ptr<VKSemaphore>> _imageAvailableSemaphores;
        std::vector<std::shared_ptr<CommandBufferRun>> _runOfFrame;

        bool _requiresSwapchainRecreation;
        uint32_t _currentFrame;
        uint32_t _imageIndex;
        std::atomic_bool _modalMode;

        VkDescriptorPool _imGuiPool;

        std::thread::id _mainThread;
        FrameInformation _currentFrameInformation;

        // region VULKAN INITIALIZATION

        void createInstance();

        bool checkValidationLayerSupport();

        std::vector<const char*> getRequiredExtensions();

        void setupDebugMessenger();

        void createSurface();

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createSwapChain();

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        void createCommandPool();

        void createSyncObjects();

        void initImGui();

        void recreateSwapChain();

        void cleanupSwapChain();

        // endregion

      public:
        VKApplication(const VKApplication& other) = delete;

        explicit VKApplication(const VKApplicationCreateInfo& info);

        ~VKApplication() override;

        void init(neon::Application* application) override;

        GLFWwindow* getWindow() const;

        [[nodiscard]] rush::Vec2i getWindowSize() const override;

        const ApplicationCreateInfo& getCreationInfo() const override;

        [[nodiscard]] FrameInformation getCurrentFrameInformation() const override;

        [[nodiscard]] CommandBuffer* getCurrentCommandBuffer() const override;

        void lockMouse(bool lock) override;

        bool isInModalMode() const override;

        void setModalMode(bool modal) override;

        Result<uint32_t, std::string> startGameLoop() override;

        void renderFrame(neon::Room* room) override;

        bool isMainThread() const override;

        void preWindowCreation();

        void postWindowCreation();

        bool preUpdate(Profiler& profiler);

        void endDraw(Profiler& pofiler);

        void finishLoop();

        void internalForceSizeValues(int32_t width, int32_t height);

        [[nodiscard]] Application* getApplication() const override;

        [[nodiscard]] uint32_t getCurrentFrame() const override;

        [[nodiscard]] uint32_t getCurrentSwapChainImage() const override;

        [[nodiscard]] uint32_t getMaxFramesInFlight() const override;

        [[nodiscard]] VkInstance getInstance() const override;

        [[nodiscard]] const VKPhysicalDevice& getPhysicalDevice() const override;

        [[nodiscard]] VKDevice* getDevice() const override;

        [[nodiscard]] VkQueue getGraphicsQueue() override;

        [[nodiscard]] VkSurfaceKHR getSurface() const;

        [[nodiscard]] VkSwapchainKHR getSwapChain() const override;

        [[nodiscard]] VkFormat getSwapChainImageFormat() const override;

        [[nodiscard]] TextureFormat getDepthImageFormat() const override;

        [[nodiscard]] VkFormat getVkDepthImageFormat() const override;

        [[nodiscard]] VkExtent2D getSwapChainExtent() const override;

        [[nodiscard]] uint32_t getSwapChainCount() const override;

        [[nodiscard]] CommandPool* getCommandPool() const override;

        [[nodiscard]] VKResourceBin* getBin() override;

        [[nodiscard]] VkDescriptorPool getImGuiPool() const override;

        [[nodiscard]] bool isRecordingCommandBuffer() const override;
    };
} // namespace neon::vulkan

#endif // NEON_VKAPPLICATION_H
