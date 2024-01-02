#include "vulkan_driver.h"

#include "utils/assert.h"
#include "vulkan_physical_device.h"
#include "vulkan_surface.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
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

#ifndef NDEBUG
// layer
const char kLayerKhronosValidation[] = "VK_LAYER_KHRONOS_validation";
// const char kLayerKhronosSynchronization2[] = "VK_LAYER_KHRONOS_synchronization2";
// const char kLayerKhronosShaderObject[] = "VK_LAYER_KHRONOS_shader_object";
// const char kLayerKhronosProfiles[] = "VK_LAYER_KHRONOS_profiles";
// const char kLayerKhronosAPIDump[] = "VK_LAYER_LUNARG_api_dump";

// debug
const char kExtensionNameExtDebugReport[] = "VK_EXT_debug_report";
const char kExtensionNameExtDebugUtils[] = "VK_EXT_debug_utils";
#endif

namespace jipu
{

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                           VkDebugUtilsMessageTypeFlagsEXT type,
                                                           const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                           void* userData)
{
    auto message = fmt::format("[{} {}]: {}", callbackData->messageIdNumber, callbackData->pMessageIdName, callbackData->pMessage);

    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        spdlog::trace(message);
    }
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        spdlog::info(message);
    }
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        spdlog::warn(message);
    }
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        spdlog::error(message);
    }
    else
    {
        spdlog::debug(message);
    }

    return VK_FALSE;
}

VulkanDriver::VulkanDriver(const DriverDescriptor& descriptor) noexcept(false)
    : Driver(descriptor)
{
    initialize();
}

VulkanDriver::~VulkanDriver()
{
#ifndef NDEBUG
    if (m_debugUtilsMessenger)
    {
        vkAPI.DestroyDebugUtilsMessengerEXT(m_instance, m_debugUtilsMessenger, nullptr);
    }
#endif
    vkAPI.DestroyInstance(m_instance, nullptr);
}

void VulkanDriver::initialize() noexcept(false)
{
#if defined(__ANDROID__) || defined(ANDROID)
    const char vulkanLibraryName[] = "libvulkan.so";
#elif defined(__linux__)
    const char vulkanLibraryName[] = "libvulkan.so.1";
#elif defined(__APPLE__)
    const char vulkanLibraryName[] = "libvulkan.dylib";
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

    gatherPhysicalDevices();
}

void VulkanDriver::createInstance() noexcept(false)
{
    // Application Information.
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    // applicationInfo.apiVersion = m_driverInfo.apiVersion;
    applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 2, 0); // TODO: check required version.

    // Create Vulkan instance.
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;

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

    VkResult result = vkAPI.CreateInstance(&instanceCreateInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failed to create VkInstance: {}", static_cast<int32_t>(result)));
    }

    VulkanDriverKnobs& driverKnobs = static_cast<VulkanDriverKnobs&>(m_driverInfo);
    if (!vkAPI.loadInstanceProcs(m_instance, driverKnobs))
    {
        throw std::runtime_error(fmt::format("Failed to load instance prosc."));
    }

#ifndef NDEBUG
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
    debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debugUtilsMessengerCreateInfo.pfnUserCallback = debugUtilsMessengerCallback;
    result = vkAPI.CreateDebugUtilsMessengerEXT(m_instance, &debugUtilsMessengerCreateInfo, nullptr, &m_debugUtilsMessenger);
    if (result != VK_SUCCESS)
    {
        spdlog::error("Failed to create debug util messager: {}", static_cast<int32_t>(result));
    }
#endif
}

void VulkanDriver::gatherPhysicalDevices() noexcept(false)
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
            spdlog::error("Failed to get instance layer properties count. {}", static_cast<int32_t>(result));
            return;
        }

        m_driverInfo.layerProperties.resize(instanceLayerCount);
        result = vkAPI.EnumerateInstanceLayerProperties(&instanceLayerCount, m_driverInfo.layerProperties.data());
        if (result != VK_SUCCESS)
        {
            spdlog::error("Failed to enumerate instance layer properties. {}", static_cast<int32_t>(result));
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
#ifndef NDEBUG
            if (strncmp(extensionProperty.extensionName, kExtensionNameExtDebugReport, VK_MAX_EXTENSION_NAME_SIZE) == 0)
            {
                m_driverInfo.debugReport = true;
            }
            if (strncmp(extensionProperty.extensionName, kExtensionNameExtDebugUtils, VK_MAX_EXTENSION_NAME_SIZE) == 0)
            {
                m_driverInfo.debugUtils = true;
            }
#endif
        }
    }
}

std::unique_ptr<Surface> VulkanDriver::createSurface(const SurfaceDescriptor& descriptor)
{
    return std::make_unique<VulkanSurface>(this, descriptor);
}

std::vector<std::unique_ptr<PhysicalDevice>> VulkanDriver::getPhysicalDevices()
{
    std::vector<std::unique_ptr<PhysicalDevice>> physicalDevices{};
    for (auto physicalDevice : m_physicalDevices)
    {
        VulkanPhysicalDeviceDescriptor descriptor{};
        descriptor.physicalDevice = physicalDevice;

        auto device = std::make_unique<VulkanPhysicalDevice>(this, descriptor);
        physicalDevices.push_back(std::move(device));
    }

    return physicalDevices;
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
            spdlog::error("Instance extension is not found. {}", requiredInstanceExtension);
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

#ifndef NDEBUG
    if (m_driverInfo.debugReport)
        requiredInstanceExtensions.push_back(kExtensionNameExtDebugReport);
    if (m_driverInfo.debugUtils)
        requiredInstanceExtensions.push_back(kExtensionNameExtDebugUtils);
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
    requiredInstanceLayers.push_back(kLayerKhronosValidation);
#endif

    spdlog::info("Required Instance layers :");
    for (const auto& layer : requiredInstanceLayers)
    {
        spdlog::info("{}{}", '\t', layer);
    }

    return requiredInstanceLayers;
}

} // namespace jipu
