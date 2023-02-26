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

class AppInfo
{
public:
    AppInfo(const char* p)
    {
        path = std::filesystem::path(p);
        dir = path.parent_path();
    }
    
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
};

std::filesystem::path AppInfo::path;
std::filesystem::path AppInfo::dir;

namespace fs = std::filesystem;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
const bool enableValidationLayers = true;
const bool enableDebugMessenger = true;
#else
const bool enableValidationLayers = false;
const bool enableDebugMessenger = false;
#endif

static std::vector<char> readFile(const std::filesystem::path& file_path)
{
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error(std::string("failed to open file! : ") + file_path.c_str());
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    if (messageType != 1)
    {
        std::cerr << "pCallbackData->pMessage : " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

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

const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

class Application
{
public:
    void run()
    {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* m_pWindow;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT m_debugMessengerUtilsCreateInfo;

    VkSurfaceKHR m_surface;
    vkt::Context m_context;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    VkSwapchainKHR m_swapChain;
    std::vector<VkImage> m_vecSwapChainImages;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;

    std::vector<VkImageView> m_vecSwapChainImageViews;

    VkShaderModule m_vertShaderModule;
    VkShaderModule m_fragShaderModule;
    VkRenderPass m_renderPass;
    VkPipelineLayout m_pipelineLayout;

    VkPipeline m_graphicsPipeline;

    std::vector<VkFramebuffer> m_vecSwapChainFramebuffers;

    VkCommandPool m_commandPool;

    std::vector<VkCommandBuffer> m_vecCommandBuffers;

    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;

private:
    void initWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan()
    {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFramebuffers();
        createCommandPool();
        createCommandBuffers();
        createSemaphores();
    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(m_pWindow))
        {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(m_context.device);
    }

    void cleanup()
    {
        vkDestroySemaphore(m_context.device, m_renderFinishedSemaphore, nullptr);
        vkDestroySemaphore(m_context.device, m_imageAvailableSemaphore, nullptr);

        vkDestroyCommandPool(m_context.device, m_commandPool, nullptr);

        for (auto framebuffer : m_vecSwapChainFramebuffers)
        {
            vkDestroyFramebuffer(m_context.device, framebuffer, nullptr);
        }

        vkDestroyPipeline(m_context.device, m_graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_context.device, m_pipelineLayout, nullptr);
        vkDestroyRenderPass(m_context.device, m_renderPass, nullptr);

        for (const VkImageView& imageView : m_vecSwapChainImageViews)
        {
            vkDestroyImageView(m_context.device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(m_context.device, m_swapChain, nullptr);

        vkDestroyDevice(m_context.device, nullptr);

        DestroyDebugUtilsMessengerEXT(m_context.instance, m_debugMessenger, nullptr);

        vkDestroySurfaceKHR(m_context.instance, m_surface, nullptr);

        vkDestroyInstance(m_context.instance, nullptr);

        glfwDestroyWindow(m_pWindow);

        glfwTerminate();
    }

    void createInstance()
    {
        const std::vector<const char*>& requiredValidationLayers = getRequiredValidationLayers();
        if (enableValidationLayers && !checkValidationLayerSupport(requiredValidationLayers))
        {
            throw std::runtime_error("validation layers requested, but not "
                                     "available for instance!");
        }

        const std::vector<const char*>& requiredInstanceExtensions = getRequiredInstanceExtensions();
        if (!checkInstanceExtensionSupport(requiredInstanceExtensions))
        {
            throw std::runtime_error("instance extensions requested, but not available!");
        }

//        VkApplicationInfo applicationInfo{};
//        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//        applicationInfo.pApplicationName = "Triangle App";
//        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//        applicationInfo.pEngineName = "Prototype";
//        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#if defined(__APPLE__)
    #if VK_HEADER_VERSION >= 216
        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif
#endif
//        instanceCreateInfo.pApplicationInfo = &applicationInfo;

        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();

        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.pNext = nullptr;
        if (enableDebugMessenger)
        {
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
            populateDefaultDebugUtilsMessengerCreateInfo(m_debugMessengerUtilsCreateInfo);
            instanceCreateInfo.pNext = (const void*)&m_debugMessengerUtilsCreateInfo;
        }

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_context.instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void pickPhysicalDevice()
    {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(m_context.instance, &physicalDeviceCount, nullptr);
        std::cout << "Physical Device Count : " << physicalDeviceCount << std::endl;
        if (physicalDeviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(m_context.instance, &physicalDeviceCount, physicalDevices.data());

        for (const VkPhysicalDevice& physicalDevice : physicalDevices)
        {
            if (isDeviceSuitable(physicalDevice))
            {
                m_context.physicalDevice = physicalDevice;
                break;
            }
        }

        if (m_context.physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice)
    {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        bool deviceExtensionsSupported = checkDeviceExtensionSupport(physicalDevice);

        bool swapChainAdequate = false;

        if (deviceExtensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
            swapChainAdequate = !swapChainSupport.surfaceFormats.empty() && !swapChainSupport.presentModes.empty();
        }

        return queueFamilyIndices.isComplete() && deviceExtensionsSupported && swapChainAdequate;
    }

    const std::vector<const char*>& getRequiredInstanceExtensions()
    {
        static std::vector<const char*> requiredInstanceExtensions;

        if (requiredInstanceExtensions.size() == 0)
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char*> glfwRequiredInstanceExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
            requiredInstanceExtensions = glfwRequiredInstanceExtensions;

            if (enableDebugMessenger)
            {
                requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
#if defined(__APPLE__)
            requiredInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

            std::cout << "Required extensions :" << std::endl;
            for (const auto& extension : requiredInstanceExtensions)
            {
                std::cout << '\t' << extension << std::endl;
            }
        }

        return requiredInstanceExtensions;
    }

    bool checkInstanceExtensionSupport(const std::vector<const char*> requiredInstanceExtensions)
    {
        uint32_t instanceExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
        std::vector<VkExtensionProperties> availableInstanceExtensions(instanceExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensions.data());

        // available instance extensions;
        std::cout << "Available Instance Extensions Count : " << availableInstanceExtensions.size() << std::endl;
        std::cout << "Available Instance Extensions : " << std::endl;
        for (const auto& availableInstanceExtension : availableInstanceExtensions)
        {
            std::cout << '\t' << availableInstanceExtension.extensionName << std::endl;
        }

        for (const auto& requiredInstanceExtension : requiredInstanceExtensions)
        {
            bool extensionFound = false;
            for (const auto& availableInstanceExtension : availableInstanceExtensions)
            {
                if (strcmp(requiredInstanceExtension, availableInstanceExtension.extensionName) == 0)
                {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound)
            {
                return false;
            }
        }

        return true;
    }

    const std::vector<const char*>& getRequiredValidationLayers()
    {
        static std::vector<const char*> requiredValidationLayers;

        if (requiredValidationLayers.size() == 0)
        {
            if (enableValidationLayers)
            {
                // requiredValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
            }

            std::cout << "Required Validataion Layers :" << std::endl;
            for (const auto& validationLayer : requiredValidationLayers)
            {
                std::cout << '\t' << validationLayer << std::endl;
            }
        }

        return requiredValidationLayers;
    }

    bool checkValidationLayerSupport(const std::vector<const char*> validationLayers)
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        std::cout << "Available Validation Layer Count : \n\t" << availableLayers.size() << std::endl;
        std::cout << "Available Aalidation Layer : " << std::endl;
        for (const VkLayerProperties& layerProperties : availableLayers)
        {
            std::cout << "\t " << layerProperties.layerName << std::endl;
        }

        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    void setupDebugMessenger()
    {
        if (!enableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDefaultDebugUtilsMessengerCreateInfo(createInfo);
        if (CreateDebugUtilsMessengerEXT(m_context.instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pDebugUtilsMessengerCreateInfoEXT,
                                          const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugUtilsMessengerEXT)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pDebugUtilsMessengerCreateInfoEXT, pAllocator, pDebugUtilsMessengerEXT);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        if (!enableValidationLayers)
            return;

        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void populateDefaultDebugUtilsMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugUtilsMessengerCreateInfo)
    {
        debugUtilsMessengerCreateInfo = {};
        debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugUtilsMessengerCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugUtilsMessengerCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugUtilsMessengerCreateInfo.pfnUserCallback = debugCallback;
        debugUtilsMessengerCreateInfo.flags = 0;
        debugUtilsMessengerCreateInfo.pNext = nullptr;
        debugUtilsMessengerCreateInfo.pUserData = nullptr; // Optional
    }

    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice)
    {
        QueueFamilyIndices queueFamilyIndices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        std::cout << queueFamilyProperties.size() << std::endl;
        for (size_t i = 0; i < queueFamilyProperties.size(); i++)
        {
            const auto& queueFamily = queueFamilyProperties[i];
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                queueFamilyIndices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_surface, &presentSupport);

            if (presentSupport)
            {
                queueFamilyIndices.presentFamily = i;
            }

            if (queueFamilyIndices.isComplete())
            {
                break;
            }
        }

        return queueFamilyIndices;
    }

    void createLogicalDevice()
    {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_context.physicalDevice);
        std::set<uint32_t> uniqueQueueFamilieIndices = { queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value() };

        std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;

        float queuePriority = 1.0f;
        for (const uint32_t queueFamilyIndex : uniqueQueueFamilieIndices)
        {
            VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
            deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
            deviceQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
            deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
        }

        VkPhysicalDeviceFeatures physicalDeviceFeatures{};

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
        deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

        // set validation layers to be compatible with older implementations:
        if (enableValidationLayers)
        {
            const std::vector<const char*>& requiredValidationLayers = getRequiredValidationLayers();
            if (enableValidationLayers && !checkValidationLayerSupport(requiredValidationLayers))
            {
                throw std::runtime_error("validation layers requested, but not "
                                         "available for device!");
            }
            deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
            deviceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
        }
        else
        {
            deviceCreateInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_context.physicalDevice, &deviceCreateInfo, nullptr, &m_context.device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_context.device, queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_context.device, queueFamilyIndices.presentFamily.value(), 0, &m_presentQueue);
    }

    void createSurface()
    {
        if (glfwCreateWindowSurface(m_context.instance, m_pWindow, nullptr, &m_surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    bool checkDeviceExtensionSupport(const VkPhysicalDevice& physicalDevice)
    {
        uint32_t deviceExtensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);

        std::vector<VkExtensionProperties> availableDeviceExtensions(deviceExtensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, availableDeviceExtensions.data());

        std::set<std::string> requiredDeviceExtensionsTemp(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());

        for (const VkExtensionProperties& availableDeviceExtension : availableDeviceExtensions)
        {
            requiredDeviceExtensionsTemp.erase(availableDeviceExtension.extensionName);
        }

        return requiredDeviceExtensionsTemp.empty();
    }

    SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& physicalDevice)
    {
        SwapChainSupportDetails swapChainSupportDetails;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &swapChainSupportDetails.surfaceCapabilities);

        uint32_t surfaceFormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, nullptr);

        if (surfaceFormatCount != 0)
        {
            swapChainSupportDetails.surfaceFormats.resize(surfaceFormatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, swapChainSupportDetails.surfaceFormats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            swapChainSupportDetails.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, swapChainSupportDetails.presentModes.data());
        }

        return swapChainSupportDetails;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats)
    {
        for (const VkSurfaceFormatKHR& availableSurfaceFormat : availableSurfaceFormats)
        {
            if (availableSurfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableSurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableSurfaceFormat;
            }
        }

        return availableSurfaceFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
    {
        if (surfaceCapabilities.currentExtent.width != UINT32_MAX)
        {
            return surfaceCapabilities.currentExtent;
        }
        else
        {
            int frameBufferWidth, frameBufferHeight;
            glfwGetFramebufferSize(m_pWindow, &frameBufferWidth, &frameBufferHeight);

            VkExtent2D actualImageExtent = { static_cast<uint32_t>(frameBufferWidth), static_cast<uint32_t>(frameBufferHeight) };

            actualImageExtent.width = std::clamp(actualImageExtent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
            actualImageExtent.height =
                std::clamp(actualImageExtent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

            return actualImageExtent;
        }
    }

    void createSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_context.physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.surfaceFormats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D imageExtent = chooseSwapExtent(swapChainSupport.surfaceCapabilities);

        uint32_t imageCount = swapChainSupport.surfaceCapabilities.minImageCount + 1;
        if (swapChainSupport.surfaceCapabilities.maxImageCount > 0 && imageCount > swapChainSupport.surfaceCapabilities.maxImageCount)
        {
            imageCount = swapChainSupport.surfaceCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = m_surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = imageExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices foundQueueFamilyIndices = findQueueFamilies(m_context.physicalDevice);
        uint32_t queueFamilyIndices[] = { foundQueueFamilyIndices.graphicsFamily.value(), foundQueueFamilyIndices.presentFamily.value() };

        if (foundQueueFamilyIndices.graphicsFamily != foundQueueFamilyIndices.presentFamily)
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;     // Optional
            swapchainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        swapchainCreateInfo.preTransform = swapChainSupport.surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;

        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_context.device, &swapchainCreateInfo, nullptr, &m_swapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(m_context.device, m_swapChain, &imageCount, nullptr);
        m_vecSwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_context.device, m_swapChain, &imageCount, m_vecSwapChainImages.data());

        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent = imageExtent;
    }

    void createImageViews()
    {
        m_vecSwapChainImageViews.resize(m_vecSwapChainImages.size());

        for (size_t i = 0; i < m_vecSwapChainImages.size(); i++)
        {
            VkImageViewCreateInfo imageViewCreateInfo{};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.image = m_vecSwapChainImages[i];
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.format = m_swapChainImageFormat;

            imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            imageViewCreateInfo.subresourceRange.levelCount = 1;
            imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            imageViewCreateInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_context.device, &imageViewCreateInfo, nullptr, &m_vecSwapChainImageViews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void createGraphicsPipeline()
    {
        const std::vector<char> vertShaderCode = readFile(AppInfo::getDir() / "vert.spv");
        const std::vector<char> fragShaderCode = readFile(AppInfo::getDir() / "frag.spv");

        m_vertShaderModule = createShaderModule(vertShaderCode);
        m_fragShaderModule = createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
        vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageInfo.module = m_vertShaderModule;
        vertexShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
        fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStageInfo.module = m_fragShaderModule;
        fragmentShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
        vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr; // Optional
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr; // Optional

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
        inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_swapChainExtent.width;
        viewport.height = (float)m_swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_swapChainExtent;

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports = &viewport;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
        rasterizationStateCreateInfo.depthBiasClamp = 0.0f;          // Optional
        rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;    // Optional

        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{};
        multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleStateCreateInfo.minSampleShading = 1.0f;          // Optional
        multisampleStateCreateInfo.pSampleMask = nullptr;            // Optional
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;      // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

        VkPipelineColorBlendStateCreateInfo colorBlendingStateCreateInfo{};
        colorBlendingStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendingStateCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendingStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlendingStateCreateInfo.attachmentCount = 1;
        colorBlendingStateCreateInfo.pAttachments = &colorBlendAttachment;
        colorBlendingStateCreateInfo.blendConstants[0] = 0.0f; // Optional
        colorBlendingStateCreateInfo.blendConstants[1] = 0.0f; // Optional
        colorBlendingStateCreateInfo.blendConstants[2] = 0.0f; // Optional
        colorBlendingStateCreateInfo.blendConstants[3] = 0.0f; // Optional

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH };

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = 2;
        dynamicStateCreateInfo.pDynamicStates = dynamicStates;

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 0;            // Optional
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;         // Optional
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;    // Optional
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(m_context.device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputStateCreateInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        pipelineInfo.pViewportState = &viewportStateCreateInfo;
        pipelineInfo.pRasterizationState = &rasterizationStateCreateInfo;
        pipelineInfo.pMultisampleState = &multisampleStateCreateInfo;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlendingStateCreateInfo;
        pipelineInfo.pDynamicState = nullptr; // Optional
        pipelineInfo.layout = m_pipelineLayout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1;              // Optional

        if (vkCreateGraphicsPipelines(m_context.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(m_context.device, m_fragShaderModule, nullptr);
        vkDestroyShaderModule(m_context.device, m_vertShaderModule, nullptr);
    }

    VkShaderModule createShaderModule(const std::vector<char>& codeBuffer)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo{};
        shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleCreateInfo.codeSize = codeBuffer.size();
        shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(codeBuffer.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_context.device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    void createRenderPass()
    {
        VkAttachmentDescription colorAttachmentDescription{};
        colorAttachmentDescription.format = m_swapChainImageFormat;
        colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentReference{};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription{};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorAttachmentReference;

        VkSubpassDependency subpassDependency{};
        subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependency.dstSubpass = 0;
        subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.srcAccessMask = 0;
        subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassCreateInfo{};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachmentDescription;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &subpassDependency;

        if (vkCreateRenderPass(m_context.device, &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void createFramebuffers()
    {
        m_vecSwapChainFramebuffers.resize(m_vecSwapChainImageViews.size());

        for (size_t i = 0; i < m_vecSwapChainImageViews.size(); i++)
        {
            VkImageView attachments[] = { m_vecSwapChainImageViews[i] };

            VkFramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = m_renderPass;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = attachments;
            framebufferCreateInfo.width = m_swapChainExtent.width;
            framebufferCreateInfo.height = m_swapChainExtent.height;
            framebufferCreateInfo.layers = 1;

            if (vkCreateFramebuffer(m_context.device, &framebufferCreateInfo, nullptr, &m_vecSwapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void createCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_context.physicalDevice);

        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        commandPoolCreateInfo.flags = 0; // Optional

        if (vkCreateCommandPool(m_context.device, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void createCommandBuffers()
    {
        m_vecCommandBuffers.resize(m_vecSwapChainFramebuffers.size());

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = m_commandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_vecCommandBuffers.size());

        if (vkAllocateCommandBuffers(m_context.device, &commandBufferAllocateInfo, m_vecCommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < m_vecCommandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo commandBufferBeginInfo{};
            commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            commandBufferBeginInfo.flags = 0;                  // Optional
            commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(m_vecCommandBuffers[i], &commandBufferBeginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_renderPass;
            renderPassInfo.framebuffer = m_vecSwapChainFramebuffers[i];

            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = m_swapChainExtent;

            VkClearValue clearValue = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearValue;

            vkCmdBeginRenderPass(m_vecCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(m_vecCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

            vkCmdDraw(m_vecCommandBuffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(m_vecCommandBuffers[i]);

            if (vkEndCommandBuffer(m_vecCommandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }

    void drawFrame()
    {
        uint32_t imageIndex;
        vkAcquireNextImageKHR(m_context.device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore };
        VkPipelineStageFlags waitPipelineStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitPipelineStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_vecCommandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { m_swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr; // Optional

        vkQueuePresentKHR(m_presentQueue, &presentInfo);

        vkQueueWaitIdle(m_presentQueue);
    }

    void createSemaphores()
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(m_context.device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(m_context.device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphores!");
        }
    }
};

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    spdlog::error("argc: {}", argc);
    spdlog::error("argv: {}", argv[0]);

    AppInfo app_info(argv[0]);

    Application app;

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
