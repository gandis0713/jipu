#include "vulkan_api.h"

#include "utils/log.h"

namespace vkt
{
bool VulkanAPI::loadGlobalProcs(DynamicLib& vulkanLib)
{
#define GET_GLOBAL_PROC(name)                                                          \
    name = reinterpret_cast<decltype(name)>(GetInstanceProcAddr(nullptr, "vk" #name)); \
    if (name == nullptr)                                                               \
    {                                                                                  \
        LOG_ERROR("Couldn't get proc vk{}", #name);                                    \
        return false;                                                                  \
    }

    if (!vulkanLib.getProc(&GetInstanceProcAddr, "vkGetInstanceProcAddr"))
    {
        LOG_ERROR("Couldn't get vkGetInstanceProcAddr");
        return false;
    }

    GET_GLOBAL_PROC(CreateInstance);
    GET_GLOBAL_PROC(EnumerateInstanceExtensionProperties);
    GET_GLOBAL_PROC(EnumerateInstanceLayerProperties);

    // Is not available in Vulkan 1.0, so allow nullptr
    EnumerateInstanceVersion = reinterpret_cast<decltype(EnumerateInstanceVersion)>(
        GetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));

    return true;
}

bool VulkanAPI::LoadInstanceProcs(VkInstance instance,
                                  const VulkanDriverKnobs& driverKnobs)
{
#define GET_INSTANCE_PROC(name)                                                         \
    name = reinterpret_cast<decltype(name)>(GetInstanceProcAddr(instance, "vk" #name)); \
    if (name == nullptr)                                                                \
    {                                                                                   \
        LOG_ERROR("Couldn't get proc vk{}", #name);                                     \
        return false;                                                                   \
    }

    // Load this proc first so that we can destroy the instance even if some other
    // GET_INSTANCE_PROC fails
    GET_INSTANCE_PROC(DestroyInstance);

    GET_INSTANCE_PROC(CreateDevice);
    GET_INSTANCE_PROC(DestroyDevice);
    GET_INSTANCE_PROC(EnumerateDeviceExtensionProperties);
    GET_INSTANCE_PROC(EnumerateDeviceLayerProperties);
    GET_INSTANCE_PROC(EnumeratePhysicalDevices);
    GET_INSTANCE_PROC(GetDeviceProcAddr);
    GET_INSTANCE_PROC(GetPhysicalDeviceFeatures);
    GET_INSTANCE_PROC(GetPhysicalDeviceFormatProperties);
    GET_INSTANCE_PROC(GetPhysicalDeviceImageFormatProperties);
    GET_INSTANCE_PROC(GetPhysicalDeviceMemoryProperties);
    GET_INSTANCE_PROC(GetPhysicalDeviceProperties);
    GET_INSTANCE_PROC(GetPhysicalDeviceQueueFamilyProperties);
    GET_INSTANCE_PROC(GetPhysicalDeviceSparseImageFormatProperties);

    if (driverKnobs.debugReport)
    {
        GET_INSTANCE_PROC(CreateDebugReportCallbackEXT);
        GET_INSTANCE_PROC(DebugReportMessageEXT);
        GET_INSTANCE_PROC(DestroyDebugReportCallbackEXT);
    }

    //     // Vulkan 1.1 is not required to report promoted extensions from 1.0
    //     if (globalInfo.externalMemoryCapabilities ||
    //         globalInfo.apiVersion >= VK_MAKE_VERSION(1, 1, 0))
    //     {
    //         GET_INSTANCE_PROC(GetPhysicalDeviceExternalBufferPropertiesKHR);
    //     }

    //     if (globalInfo.externalSemaphoreCapabilities ||
    //         globalInfo.apiVersion >= VK_MAKE_VERSION(1, 1, 0))
    //     {
    //         GET_INSTANCE_PROC(GetPhysicalDeviceExternalSemaphorePropertiesKHR);
    //     }

    //     if (globalInfo.getPhysicalDeviceProperties2 ||
    //         globalInfo.apiVersion >= VK_MAKE_VERSION(1, 1, 0))
    //     {
    //         GET_INSTANCE_PROC(GetPhysicalDeviceFeatures2KHR);
    //         GET_INSTANCE_PROC(GetPhysicalDeviceProperties2KHR);
    //         GET_INSTANCE_PROC(GetPhysicalDeviceFormatProperties2KHR);
    //         GET_INSTANCE_PROC(GetPhysicalDeviceImageFormatProperties2KHR);
    //         GET_INSTANCE_PROC(GetPhysicalDeviceQueueFamilyProperties2KHR);
    //         GET_INSTANCE_PROC(GetPhysicalDeviceMemoryProperties2KHR);
    //         GET_INSTANCE_PROC(GetPhysicalDeviceSparseImageFormatProperties2KHR);
    //     }

    //     if (globalInfo.surface)
    //     {
    //         GET_INSTANCE_PROC(DestroySurfaceKHR);
    //         GET_INSTANCE_PROC(GetPhysicalDeviceSurfaceSupportKHR);
    //         GET_INSTANCE_PROC(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    //         GET_INSTANCE_PROC(GetPhysicalDeviceSurfaceFormatsKHR);
    //         GET_INSTANCE_PROC(GetPhysicalDeviceSurfacePresentModesKHR);
    //     }

    // #ifdef VK_USE_PLATFORM_FUCHSIA
    //     if (globalInfo.fuchsiaImagePipeSurface)
    //     {
    //         GET_INSTANCE_PROC(CreateImagePipeSurfaceFUCHSIA);
    //     }
    // #endif

    return true;
}

} // namespace vkt