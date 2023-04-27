#pragma once

// #define GLFW_INCLUDE_VULKAN
#include "gpu/adapter.h"
#include "gpu/context.h"
#include "gpu/device.h"
#include "gpu/driver.h"
#include "gpu/platform.h"
#include "gpu/queue.h"
#include "gpu/surface.h"
#include "gpu/swapchain.h"
#include "gpu/vulkan/vulkan_framebuffer.h"
#include "gpu/vulkan/vulkan_render_pass.h"
#include "utils/log.h"
#include <GLFW/glfw3.h>

#include "gpu/pipeline.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

namespace vkt
{

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Window;

class Application
{
public:
    Application() = default;
    Application(int argc, char** argv);

public:
    void run();

public:
    static std::filesystem::path getPath()
    {
        return path;
    }
    static std::filesystem::path getDir()
    {
        return dir;
    }

private:
    static std::filesystem::path path;
    static std::filesystem::path dir;

private:
    void initWindow();
    void initVulkan();

    void mainLoop();
    void cleanup();

    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

    void createSurface();
    void createSwapChain();
    //    void createImageViews();
    void createGraphicsPipeline();
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSemaphores();

    void drawFrame();

    // TODO: remove
    // Validation layer
    // const std::vector<const char*>& getRequiredValidationLayers();
    // bool checkValidationLayerSupport(const std::vector<const char*> validationLayers);
    // void setupDebugMessenger();
    // VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT*
    // pDebugUtilsMessengerCreateInfoEXT,
    //                                       const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT*
    //                                       pDebugUtilsMessengerEXT);
    // void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const
    // VkAllocationCallbacks* pAllocator); void
    // populateDefaultDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo);

private:
    Window* m_window;

    // debug
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT m_debugMessengerUtilsCreateInfo;

    // wrapper
    std::unique_ptr<Driver> m_driver{ nullptr };
    std::unique_ptr<Adapter> m_adapter{ nullptr };

    std::unique_ptr<Surface> m_surface{ nullptr };
    std::unique_ptr<Device> m_device{ nullptr };

    std::unique_ptr<SwapChain> m_swapChain = nullptr;
    std::unique_ptr<Pipeline> m_pipeline{ nullptr };

    std::unique_ptr<Queue> m_graphicsQueue{ nullptr };
    std::unique_ptr<Queue> m_presentQueue{ nullptr };

    // command
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_vecCommandBuffers;

    // sync
    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;

    VulkanRenderPassDescriptor m_renderPassDescriptor{};
    std::vector<VulkanFramebufferDescriptor> m_framebufferDescriptors{};
};

} // namespace vkt
