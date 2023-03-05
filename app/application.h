#pragma once

// #define GLFW_INCLUDE_VULKAN
#include "utils/log.h"
#include "vk/context.h"
#include "vk/surface.h"
#include "vk/swap_chain.h"
#include <GLFW/glfw3.h>

#include "vk/pipeline.h"
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
    void run();

    Application() = default;
    Application(int argc, char** argv)
    {
        path = std::filesystem::path(argv[0]);
        dir = path.parent_path();
    }

    static std::filesystem::path getPath() { return path; }
    static std::filesystem::path getDir() { return dir; }

private:
    static std::filesystem::path path;
    static std::filesystem::path dir;

private:
    Window* m_window;

    // debug
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT m_debugMessengerUtilsCreateInfo;

    // surface
    std::shared_ptr<Surface> m_surface = nullptr;

    // swap chain
    std::shared_ptr<SwapChain> m_swapChain = nullptr;

    // frame buffers
    std::vector<VkFramebuffer> m_vecSwapChainFramebuffers;

    // vulkan context
    vkt::Context m_context;

    VkRenderPass m_renderPass;

    // pipeline
    Pipeline m_pipeline;

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

    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice);

    void createLogicalDevice();
    void createSurface();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

    void createSwapChain();
    //    void createImageViews();
    void createGraphicsPipeline();
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSemaphores();

    void drawFrame();

    // Validation layer
    const std::vector<const char*>& getRequiredValidationLayers();
    bool checkValidationLayerSupport(const std::vector<const char*> validationLayers);
    void setupDebugMessenger();
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pDebugUtilsMessengerCreateInfoEXT,
                                          const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugUtilsMessengerEXT);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    void populateDefaultDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo);
};

} // namespace vkt
