#include "vulkan_driver.h"

#include "utils/assert.h"
#include "utils/log.h"
#include "vulkan_physical_device.h"
#include "vulkan_surface.h"

#include <fmt/format.h>
#include <stdexcept>

// surface
const char kExtensionNameKhrSurface[] = "VK_KHR_surface";
const char kExtensionNameMvkMacosSurface[] = "VK_MVK_macos_surface";
const char kExtensionNameExtMetalSurface[] = "VK_EXT_metal_surface";
const char kExtensionNameKhrWin32Surface[] = "VK_KHR_win32_surface";
const char kExtensionNameKhrAndroidSurface[] = "VK_KHR_android_surface";
const char kExtensionNameKhrXcbSurface[] = "VK_KHR_xcb_surface";
// const char kExtensionNameKhrWaylandSurface[] = "VK_KHR_wayland_surface";
// const char kExtensionNameKhrXlibSurface[] = "VK_KHR_xlib_surface";

// swapchain
const char kExtensionNameKhrSwapchain[] = "VK_KHR_swapchain";

// layer
const char kLayerKhronosValidation[] = "VK_LAYER_KHRONOS_validation";

namespace vkt
{

VulkanDriver::VulkanDriver(const DriverDescriptor& descriptor) noexcept(false)
    : Driver(descriptor)
{
    initialize();
}

VulkanDriver::~VulkanDriver()
{
    vkAPI.DestroyInstance(m_instance, nullptr);
}

void VulkanDriver::initialize() noexcept(false)
{
#if defined(__ANDROID__) || defined(ANDROID)
    const char vulkanLibraryName[] = "libvulkan.so";
#elif defined(__linux__)
    const char vulkanLibraryName[] = "libvulkan.so.1";
#elif defined(__APPLE__)
    const char vulkanLibraryName[] = "libvulkan.1.dylib";
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

    spdlog::info("Vulkan API Version: {}.{}.{}",
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
    if (m_driverInfo.apiVersion >= VK_MAKE_VERSION(1, 3, 216))
    {
        instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }
#endif
    const std::vector<const char*> requiredInstanceLayers = getRequiredInstanceLayers();
    if (!checkInstanceLayerSupport(requiredInstanceLayers))
    {
        throw std::runtime_error("instance layers requested, but not available!");
    }

    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredInstanceLayers.size());
    instanceCreateInfo.ppEnabledLayerNames = requiredInstanceLayers.data();

    const std::vector<const char*> requiredInstanceExtensions = getRequiredInstanceExtensions();
    if (!checkInstanceExtensionSupport(requiredInstanceExtensions))
    {
        throw std::runtime_error("instance extensions requested, but not available!");
    }

    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();
    instanceCreateInfo.pNext = nullptr;

    // Application Information.
    {
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = m_driverInfo.apiVersion;

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

    VkResult result = vkAPI.CreateInstance(&instanceCreateInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failed to create VkInstance: {}", result));
    }
}

void VulkanDriver::createPhysicalDevices() noexcept(false)
{
    uint32_t physicalDeviceCount = 0;
    vkAPI.EnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);

    spdlog::info("Physical Device Count: {}", physicalDeviceCount);
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
            spdlog::error("Failed to get instance layer properties count. {}", result);
            return;
        }

        m_driverInfo.layerProperties.resize(instanceLayerCount);
        result = vkAPI.EnumerateInstanceLayerProperties(&instanceLayerCount, m_driverInfo.layerProperties.data());
        if (result != VK_SUCCESS)
        {
            spdlog::error("Failed to enumerate instance layer properties. {}", result);
            return;
        }

        for (const auto& layerProperty : m_driverInfo.layerProperties)
        {
            // TODO: set driver knobs for layer
            spdlog::info("Instance Layer Name: {}", layerProperty.layerName);
        }
    }

    // Gather instance extension properties.
    {
        uint32_t instanceExtensionCount = 0;
        VkResult result = vkAPI.EnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
        if (result != VK_SUCCESS && result != VK_INCOMPLETE)
        {
            spdlog::error("Failed to get instance extension properties count.");
            return;
        }

        m_driverInfo.extensionProperties.resize(instanceExtensionCount);
        result = vkAPI.EnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, m_driverInfo.extensionProperties.data());
        if (result != VK_SUCCESS)
        {
            spdlog::error("Failed to enumerate instance extension properties.");
            return;
        }

        for (const auto& extensionProperty : m_driverInfo.extensionProperties)
        {
            // TODO: set driver knobs for extension
            spdlog::info("Instance Extension Name: {}, SpecVersion: {}", extensionProperty.extensionName, extensionProperty.specVersion);

            if (strncmp(extensionProperty.extensionName, kExtensionNameKhrSurface, VK_MAX_EXTENSION_NAME_SIZE) == 0)
            {
                m_driverInfo.surface = true;
            }

            if (strncmp(extensionProperty.extensionName, kExtensionNameKhrAndroidSurface, VK_MAX_EXTENSION_NAME_SIZE) == 0)
            {
                m_driverInfo.androidSurface = true;
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

std::unique_ptr<Surface> VulkanDriver::createSurface(const SurfaceDescriptor& descriptor)
{
    return std::make_unique<VulkanSurface>(this, descriptor);
}

std::unique_ptr<PhysicalDevice> VulkanDriver::createPhysicalDevice(const PhysicalDeviceDescriptor& descriptor)
{
    return std::make_unique<VulkanPhysicalDevice>(this, descriptor);
}

VkInstance VulkanDriver::getVkInstance() const
{
    return m_instance;
}

const std::vector<VkPhysicalDevice>& VulkanDriver::getVkPhysicalDevices() const
{
    return m_physicalDevices;
}

VkPhysicalDevice VulkanDriver::getVkPhysicalDevice(uint32_t index) const
{
    assert(index < m_physicalDevices.size());

    return m_physicalDevices[index];
}

const VulkanDriverInfo& VulkanDriver::getDriverInfo() const
{
    return m_driverInfo;
}

bool VulkanDriver::checkInstanceExtensionSupport(const std::vector<const char*> requiredInstanceExtensions)
{
    for (const auto& requiredInstanceExtension : requiredInstanceExtensions)
    {
        bool extensionFound = false;
        for (const auto& availableInstanceExtension : m_driverInfo.extensionProperties)
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

    requiredInstanceExtensions.push_back(kExtensionNameKhrSurface);

#if defined(__ANDROID__) || defined(ANDROID)
    requiredInstanceExtensions.push_back(kExtensionNameKhrAndroidSurface);
#elif defined(__linux__)
    requiredInstanceExtensions.push_back(kExtensionNameKhrXcbSurface);
#elif defined(_WIN32)
    requiredInstanceExtensions.push_back(kExtensionNameKhrWin32Surface);
#elif defined(__APPLE__)
    #if defined(VK_USE_PLATFORM_MACOS_MVK)
    requiredInstanceExtensions.push_back(kExtensionNameMvkMacosSurface);
    #elif defined(VK_USE_PLATFORM_METAL_EXT)
    requiredInstanceExtensions.push_back(kExtensionNameExtMetalSurface);
    #endif
#endif

#if VK_HEADER_VERSION >= 216
    if (m_driverInfo.apiVersion >= VK_MAKE_VERSION(1, 3, 216))
    {
        requiredInstanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }
#endif

    spdlog::info("Required Instance extensions :");
    for (const auto& extension : requiredInstanceExtensions)
    {
        spdlog::info("{}{}", '\t', extension);
    }

    // TODO: Debug Utils
    // requiredInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return requiredInstanceExtensions;
}

bool VulkanDriver::checkInstanceLayerSupport(const std::vector<const char*> requiredInstanceLayers)
{
    for (const auto& requiredInstanceLayer : requiredInstanceLayers)
    {
        bool layerFound = false;
        for (const auto& availableInstanceLayer : m_driverInfo.layerProperties)
        {
            if (strcmp(requiredInstanceLayer, availableInstanceLayer.layerName) == 0)
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

const std::vector<const char*> VulkanDriver::getRequiredInstanceLayers()
{
    std::vector<const char*> requiredInstanceLayers{};

#ifndef NDEBUG
//    requiredInstanceLayers.push_back(kLayerKhronosValidation);
#endif

    return requiredInstanceLayers;
}

} // namespace vkt
