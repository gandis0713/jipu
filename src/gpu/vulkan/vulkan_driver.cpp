#include "vulkan_driver.h"

#include "utils/assert.h"
#include "utils/log.h"
#include "vulkan_adapter.h"
#include "vulkan_allocation.h"

#include <fmt/format.h>
#include <stdexcept>

const char kExtensionNameKhrSurface[] = "VK_KHR_surface";
const char kExtensionNameMvkMacosSurface[] = "VK_MVK_macos_surface";
const char kExtensionNameKhrWaylandSurface[] = "VK_KHR_wayland_surface";
const char kExtensionNameKhrWin32Surface[] = "VK_KHR_win32_surface";
const char kExtensionNameKhrXcbSurface[] = "VK_KHR_xcb_surface";
const char kExtensionNameKhrXlibSurface[] = "VK_KHR_xlib_surface";
const char kExtensionNameExtMetalSurface[] = "VK_EXT_metal_surface";

const char kExtensionNameKhrSwapchain[] = "VK_KHR_swapchain";

// static VkPhysicalDevice selectPhysicalDevice(const std::vector<VkPhysicalDevice>& physicalDevices,
//                                              VkQueueFlags queueFlags = VK_QUEUE_GRAPHICS_BIT)
// {
//     for (const VkPhysicalDevice& candidatePhysicalDevice : physicalDevices)
//     {
//         uint32_t queueFamilyCount = 0;
//         vkGetPhysicalDeviceQueueFamilyProperties(candidatePhysicalDevice, &queueFamilyCount, nullptr);

//         std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
//         vkGetPhysicalDeviceQueueFamilyProperties(candidatePhysicalDevice, &queueFamilyCount,
//                                                  queueFamilyProperties.data());

//         LOG_INFO("Queue Family Size: {}", queueFamilyProperties.size());
//         for (auto i = 0; i < queueFamilyProperties.size(); ++i)
//         {

//             // check graphic family
//             const auto& queueFamily = queueFamilyProperties[i];

//             LOG_INFO("Queue Family Index: {0}, queueFlags: {1:b}", i, queueFamily.queueFlags);

//             const int family = queueFamily.queueFlags & queueFlags;

//             if (family == queueFlags)
//             {
//                 return candidatePhysicalDevice;
//             }

//             // TODO: check support swap chain

//             // TODO: check preset???
//         }
//     }

//     throw std::runtime_error("Failed to find a suitable GPU!");

//     return VK_NULL_HANDLE;
// }

namespace vkt
{

VulkanDriver::VulkanDriver(DriverDescriptor descriptor) noexcept(false)
    : Driver()
{
    initialize();
}

VulkanDriver::~VulkanDriver()
{
    // TODO: destroy instance.
    LOG_TRACE(__func__);
}

void VulkanDriver::initialize() noexcept(false)
{
#if defined(__linux__)
    const char vulkanLibraryName[] = "libvulkan.so.1";
#elif defined(__APPLE__)
    const char vulkanLibraryName[] = "libMoltenVK.dylib";
#elif defined(WIN32)
    const char vulkanLibraryName[] = "vulkan-1.dll";
#endif

    if (!m_vulkanLib.open(vulkanLibraryName))
    {
        throw std::runtime_error(fmt::format("Failed to open vulkan library: {}", vulkanLibraryName));
    }

    if (!vkAPI.loadDriverProcs(&m_vulkanLib))
    {
        throw std::runtime_error(fmt::format("Failed to load driver prosc in vulkan library: {}", vulkanLibraryName));
    }

    if (vkAPI.EnumerateInstanceVersion != nullptr)
    {
        vkAPI.EnumerateInstanceVersion(&m_driverInfo.apiVersion);
    }

    LOG_INFO("Vulkan API Version: {}.{}.{}",
             VK_API_VERSION_MAJOR(m_driverInfo.apiVersion),
             VK_API_VERSION_MINOR(m_driverInfo.apiVersion),
             VK_API_VERSION_PATCH(m_driverInfo.apiVersion));

    gatherDriverInfo();

    createInstance();

    VulkanDriverKnobs& driverKnobs = static_cast<VulkanDriverKnobs&>(m_driverInfo);
    if (!vkAPI.loadInstanceProcs(m_instance, driverKnobs))
    {
        throw std::runtime_error(fmt::format("Failed to load instance prosc."));
    }

    createPhysicalDevices();
}

void VulkanDriver::createInstance() noexcept(false)
{
    // Create Vulkan instance.
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#if VK_HEADER_VERSION >= 216
    if (m_driverInfo.apiVersion >= VK_MAKE_API_VERSION(0, 1, 3, 216))
    {
        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }
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

    VkResult result = vkAPI.CreateInstance(&instanceCreateInfo, vkt::VK_ALLOC_CB, &m_instance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failed to create VkInstance: {}", result));
    }
}

void VulkanDriver::createPhysicalDevices() noexcept(false)
{
    uint32_t physicalDeviceCount = 0;
    vkAPI.EnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);

    LOG_INFO("Physical Device Count: {}", physicalDeviceCount);
    if (physicalDeviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    m_physicalDevices.resize(physicalDeviceCount);
    vkAPI.EnumeratePhysicalDevices(m_instance, &physicalDeviceCount, m_physicalDevices.data());
    if (m_physicalDevices.empty())
    {
        throw std::runtime_error("There is no physical device.");
    }
}

void VulkanDriver::gatherDriverInfo()
{
    // Gather instance layer properties.
    {
        uint32_t instanceLayerCount = 0;
        VkResult result = vkAPI.EnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        if (result != VK_SUCCESS && result != VK_INCOMPLETE)
        {
            LOG_ERROR("Failed to get instance layer properties count. {}", result);
            return;
        }

        m_driverInfo.layerProperties.resize(instanceLayerCount);
        result = vkAPI.EnumerateInstanceLayerProperties(&instanceLayerCount, m_driverInfo.layerProperties.data());
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Failed to enumerate instance layer properties. {}", result);
            return;
        }

        for (const auto& layerProperty : m_driverInfo.layerProperties)
        {
            // TODO: set driver knobs for layer
            LOG_INFO("Instance Layer Name: {}", layerProperty.layerName);
        }
    }

    // Gather instance extension properties.
    {
        uint32_t instanceExtensionCount = 0;
        VkResult result = vkAPI.EnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
        if (result != VK_SUCCESS && result != VK_INCOMPLETE)
        {
            LOG_ERROR("Failed to get instance extension properties count.");
            return;
        }

        m_driverInfo.extensionProperties.resize(instanceExtensionCount);
        result = vkAPI.EnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, m_driverInfo.extensionProperties.data());
        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Failed to enumerate instance extension properties.");
            return;
        }

        for (const auto& extensionProperty : m_driverInfo.extensionProperties)
        {
            // TODO: set driver knobs for extension
            LOG_INFO("Instance Extension Name: {}, SpecVersion: {}", extensionProperty.extensionName, extensionProperty.specVersion);

            if (strncmp(extensionProperty.extensionName, kExtensionNameKhrSurface, VK_MAX_EXTENSION_NAME_SIZE) == 0)
            {
                m_driverInfo.surface = true;
            }
            if (strncmp(extensionProperty.extensionName, kExtensionNameExtMetalSurface, VK_MAX_EXTENSION_NAME_SIZE) == 0)
            {
                m_driverInfo.metalSurface = true;
            }
            if (strncmp(extensionProperty.extensionName, kExtensionNameMvkMacosSurface, VK_MAX_EXTENSION_NAME_SIZE) == 0)
            {
                m_driverInfo.macosSurface = true;
            }
            if (strncmp(extensionProperty.extensionName, kExtensionNameKhrWin32Surface, VK_MAX_EXTENSION_NAME_SIZE) == 0)
            {
                m_driverInfo.win32Surface = true;
            }
        }
    }
}

bool VulkanDriver::checkInstanceExtensionSupport(const std::vector<const char*> requiredInstanceExtensions)
{
    uint32_t instanceExtensionCount = 0;
    vkAPI.EnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> availableInstanceExtensions(instanceExtensionCount);
    vkAPI.EnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensions.data());

    // available instance extensions;
    LOG_INFO("Available Instance Extensions Count: {}", availableInstanceExtensions.size());
    LOG_INFO("Available Instance Extensions : ");
    for (const auto& availableInstanceExtension : availableInstanceExtensions)
    {
        LOG_INFO("{}{}", '\t', availableInstanceExtension.extensionName);
    }

    LOG_INFO("Required Instance Extensions Count: {}", requiredInstanceExtensions.size());
    LOG_INFO("Required Instance Extensions : ");
    for (const auto& requiredInstanceExtension : requiredInstanceExtensions)
    {
        LOG_INFO("{}{}", '\t', requiredInstanceExtension);
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

const std::vector<const char*> VulkanDriver::getRequiredInstanceExtensions()
{
    std::vector<const char*> requiredInstanceExtensions{};

    requiredInstanceExtensions.push_back("VK_KHR_surface");
    /*
        https://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
        https://sourceforge.net/p/predef/wiki/OperatingSystems/
    */

#if defined(__linux__)
    requiredInstanceExtensions.push_back("VK_KHR_xcb_surface"); // for glfw on linux(ubuntu)
#elif defined(_WIN32)
    requiredInstanceExtensions.push_back("VK_KHR_win32_surface");
#elif defined(__APPLE__)
    #if defined(VK_USE_PLATFORM_MACOS_MVK)
    requiredInstanceExtensions.push_back("VK_MVK_macos_surface");
    #elif defined(VK_USE_PLATFORM_METAL_EXT)
    requiredInstanceExtensions.push_back("VK_EXT_metal_surface");
    #endif
#endif

#if VK_HEADER_VERSION >= 216
    if (m_driverInfo.apiVersion >= VK_MAKE_API_VERSION(0, 1, 3, 216))
    {
        requiredInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }
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

std::unique_ptr<Adapter> VulkanDriver::createAdapter(AdapterDescriptor descriptor)
{
    return std::make_unique<VulkanAdapter>(this, descriptor);
}

VkInstance VulkanDriver::getInstance() const
{
    return m_instance;
}

const std::vector<VkPhysicalDevice>& VulkanDriver::getPhysicalDevices() const
{
    return m_physicalDevices;
}

VkPhysicalDevice VulkanDriver::getPhysicalDevice(uint32_t index) const
{
    assert(index < m_physicalDevices.size());

    return m_physicalDevices[index];
}

const VulkanDriverInfo& VulkanDriver::getDriverInfo() const
{
    return m_driverInfo;
}

} // namespace vkt
