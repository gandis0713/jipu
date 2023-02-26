#pragma once

// #define GLFW_INCLUDE_VULKAN
#include "vk/context.h"
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <vector>

namespace fmt
{
template <> struct formatter<std::filesystem::path>
{
    constexpr auto parse(format_parse_context& context) { return context.begin(); }

    template <typename T> auto format(const std::filesystem::path& path, T& t) const { return fmt::format_to(t.out(), "{:s}", path.c_str()); }
};
} // namespace fmt

namespace vkt
{

class AppInfo
{
public:
    AppInfo(const char* p)
    {
        path = std::filesystem::path(p);
        dir = path.parent_path();
    }

    static std::filesystem::path getPath() { return path; }

    static std::filesystem::path getDir() { return dir; }

private:
    static std::filesystem::path path;
    static std::filesystem::path dir;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Application
{
public:
    void run();

private:
    GLFWwindow* m_pWindow;

    // Debug
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT m_debugMessengerUtilsCreateInfo;

    // Surface
    VkSurfaceKHR m_surface;

    // swap chain
    std::vector<VkImage> m_vecSwapChainImages;
    VkSwapchainKHR m_swapChain;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImageView> m_vecSwapChainImageViews;
    std::vector<VkFramebuffer> m_vecSwapChainFramebuffers;

    //
    vkt::Context m_context;

    // physical device
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;

    // shader module
    VkShaderModule m_vertShaderModule;
    VkShaderModule m_fragShaderModule;

    // pipeline
    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;

    // command
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_vecCommandBuffers;

    // sync
    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;

private:
    void initWindow();
    void initVulkan();

    void mainLoop();
    void cleanup();

    void createInstance();

    void pickPhysicalDevice();
    bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice);
    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice);

    void createLogicalDevice();
    void createSurface();

    VkShaderModule createShaderModule(const std::vector<char>& codeBuffer);

    SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& physicalDevice);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

    void createSwapChain();
    void createImageViews();
    void createGraphicsPipeline();
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSemaphores();

    void drawFrame();

    // Validation layer
    const std::vector<const char*>& getRequiredInstanceExtensions();
    bool checkInstanceExtensionSupport(const std::vector<const char*> requiredInstanceExtensions);
    const std::vector<const char*>& getRequiredValidationLayers();
    bool checkDeviceExtensionSupport(const VkPhysicalDevice& physicalDevice);

    bool checkValidationLayerSupport(const std::vector<const char*> validationLayers);
    void setupDebugMessenger();
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pDebugUtilsMessengerCreateInfoEXT,
                                          const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugUtilsMessengerEXT);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    void populateDefaultDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo);
};

} // namespace vkt