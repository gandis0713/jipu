#include "driver.h"
#include "utils/log.h"

#include "vk/allocation.h"

#include <set>

static const std::vector<const char*> getRequiredInstanceExtensions()
{
    std::vector<const char*> requiredInstanceExtensions{};

    requiredInstanceExtensions.push_back("VK_KHR_surface");
    /*
        https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
        https://sourceforge.net/p/predef/wiki/OperatingSystems/
    */

#if defined(__linux__)
    requiredInstanceExtensions.push_back("VK_KHR_xcb_surface"); // for glfw on linux(ubuntu)
#elif defined(_WIN64)
    requiredInstanceExtensions.push_back("VK_KHR_win32_surface");
#elif defined(__APPLE__)
    #if defined(VK_USE_PLATFORM_MACOS_MVK)
    requiredInstanceExtensions.push_back("VK_MVK_macos_surface");
    #elif defined(VK_USE_PLATFORM_METAL_EXT)
    requiredInstanceExtensions.push_back("VK_EXT_metal_surface");
    #endif
#endif

#if defined(__APPLE__)
    #if VK_HEADER_VERSION >= 216
    requiredInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    #endif
#endif

    LOG_INFO("Required Instance extensions :");
    for (const auto& extension : requiredInstanceExtensions)
    {
        LOG_INFO("{}{}", '\t', extension);
    }

    // TODO: Debug Utils
    // requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return requiredInstanceExtensions;
}

static bool checkInstanceExtensionSupport(const std::vector<const char*> requiredInstanceExtensions)
{
    uint32_t instanceExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> availableInstanceExtensions(instanceExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensions.data());

    // available instance extensions;
    LOG_INFO("Available Instance Extensions Count: {}", availableInstanceExtensions.size());
    LOG_INFO("Available Instance Extensions : ");
    for (const auto& availableInstanceExtension : availableInstanceExtensions)
    {
        LOG_INFO("{}{}", '\t', availableInstanceExtension.extensionName);
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

static VkInstance createInstance()
{
    // Create Vulkan instance.
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#if defined(__APPLE__)
    #if VK_HEADER_VERSION >= 216
    instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif
#endif

    const std::vector<const char*>& requiredInstanceExtensions = getRequiredInstanceExtensions();
    if (!checkInstanceExtensionSupport(requiredInstanceExtensions))
    {
        throw std::runtime_error("instance extensions requested, but not available!");
    }

    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();

    // TODO: layer
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.pNext = nullptr;

    // Application Information shoulb be used in instance??
    {
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

        instanceCreateInfo.pApplicationInfo = &applicationInfo;
    }

    // TODO: Debug
    // if (enableDebugMessenger)
    // {
    //     instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
    //     instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
    //     populateDefaultDebugUtilsMessengerCreateInfo(m_debugMessengerUtilsCreateInfo);
    //     instanceCreateInfo.pNext = (const void*)&m_debugMessengerUtilsCreateInfo;
    // }

    VkInstance instance{};
    VkResult result = vkCreateInstance(&instanceCreateInfo, vkt::VK_ALLOC_CB, &instance);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("Failed to create VkInstance: {}", result);
    }

    return instance;
}

static std::vector<VkPhysicalDevice> createPhysicalDevices(VkInstance instance)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    LOG_INFO("Physical Device Count: {}", physicalDeviceCount);
    if (physicalDeviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    return physicalDevices;
}

static VkPhysicalDevice selectPhysicalDevice(const std::vector<VkPhysicalDevice>& physicalDevices, VkQueueFlags queueFlags = VK_QUEUE_GRAPHICS_BIT)
{
    for (const VkPhysicalDevice& candidatePhysicalDevice : physicalDevices)
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(candidatePhysicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(candidatePhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        LOG_INFO("Queue Family Size: {}", queueFamilyProperties.size());
        for (auto i = 0; i < queueFamilyProperties.size(); ++i)
        {

            // check graphic family
            const auto& queueFamily = queueFamilyProperties[i];

            LOG_INFO("Queue Family Index: {0}, queueFlags: {1:b}", i, queueFamily.queueFlags);

            const int family = queueFamily.queueFlags & queueFlags;

            if (family == queueFlags)
            {
                return candidatePhysicalDevice;
            }

            // TODO: check support swap chain

            // TODO: check preset???
        }
    }

    throw std::runtime_error("Failed to find a suitable GPU!");

    return VK_NULL_HANDLE;
}

namespace vkt
{

Driver::Driver(DriverCreateInfo info) : m_platform(std::move(info.platform))
{
    m_instance = createInstance();
    if (m_instance == nullptr)
    {
        return;
    }

    m_physicalDevices = createPhysicalDevices(m_instance);
    if (m_physicalDevices.empty())
    {
        return;
    }

    // // select physical device
    // VkQueueFlagBits queueFlagBits = VK_QUEUE_GRAPHICS_BIT;
    // physicalDevice = selectPhysicalDevice(m_physicalDevices, queueFlagBits);
    // if (physicalDevice == nullptr)
    // {
    //     return;
    // }

    // // create device
    // QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(physicalDevice);
    // device = createDevice(physicalDevice, queueFamilyIndices);
    // if (device == nullptr)
    // {
    //     return;
    // }

    // // create graphics queue
    // vkGetDeviceQueue(device, queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);

    // // create present queue
    // vkGetDeviceQueue(device, queueFamilyIndices.presentFamily.value(), 0, &presentQueue);
}

Driver::~Driver() { terminate(); }

std::vector<std::unique_ptr<Adapter>> generateAdapters() { return {}; }

void Driver::terminate()
{
    // TODO: platfrom

    // m_context.finalize();
}

} // namespace vkt