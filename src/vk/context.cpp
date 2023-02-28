#include "context.h"
#include "allocation.h"

#include <spdlog/spdlog.h>
#include <vector>

static bool checkInstanceExtensionSupport(const std::vector<const char*> requiredInstanceExtensions)
{
    uint32_t instanceExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> availableInstanceExtensions(instanceExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensions.data());

    // available instance extensions;
    spdlog::info("Available Instance Extensions Count: {}", availableInstanceExtensions.size());
    spdlog::info("Available Instance Extensions : ");
    for (const auto& availableInstanceExtension : availableInstanceExtensions)
    {
        spdlog::info("{}{}", '\t', availableInstanceExtension.extensionName);
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

static const std::vector<const char*>& getRequiredInstanceExtensions()
{
    static std::vector<const char*> requiredInstanceExtensions;

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

    spdlog::info("Required Instance extensions :");
    for (const auto& extension : requiredInstanceExtensions)
    {
        spdlog::info("{}{}", '\t', extension);
    }

    // // Debug Utils
    // requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return requiredInstanceExtensions;
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

    // // Debug
    // if (enableDebugMessenger)
    // {
    //     instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
    //     instanceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
    //     populateDefaultDebugUtilsMessengerCreateInfo(m_debugMessengerUtilsCreateInfo);
    //     instanceCreateInfo.pNext = (const void*)&m_debugMessengerUtilsCreateInfo;
    // }

    VkInstance instance;
    VkResult result = vkCreateInstance(&instanceCreateInfo, vkt::VK_ALLOC_CB, &instance);

    return instance;
}

namespace vkt
{

void Context::initialize() { instance = createInstance(); }

void Context::finalize()
{
    if (instance == nullptr)
    {
        spdlog::warn("VkInstance is nullptr.");
        return;
    }

    // VkInstance
    {
        vkDestroyInstance(instance, VK_ALLOC_CB);
        instance = nullptr;
    }
}

} // namespace vkt