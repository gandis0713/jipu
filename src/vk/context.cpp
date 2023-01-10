#include "context.h"
#include <spdlog/spdlog.h>
#include <vector>

namespace vkt
{

// struct QueueFamilyIndices
// {
//     std::optional<uint32_t> graphicsFamily;
//     std::optional<uint32_t> presentFamily;

//     bool isComplete()
//     {
//         return graphicsFamily.has_value() && presentFamily.has_value();
//     }
// };

bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice)
{
    // QueueFamilyIndices queueFamilyIndices =
    // findQueueFamilies(physicalDevice);

    // bool deviceExtensionsSupported =
    //     checkDeviceExtensionSupport(physicalDevice);

    // bool swapChainAdequate = false;

    // if (deviceExtensionsSupported)
    // {
    //     SwapChainSupportDetails swapChainSupport =
    //         querySwapChainSupport(physicalDevice);
    //     swapChainAdequate = !swapChainSupport.surfaceFormats.empty() &&
    //                         !swapChainSupport.presentModes.empty();
    // }

    // return queueFamilyIndices.isComplete() && deviceExtensionsSupported &&
    //        swapChainAdequate;

    return true;
}

void Context::selectPhysicalDevice() noexcept
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    spdlog::debug("The count of physical device : {}", physicalDeviceCount);

    if (physicalDeviceCount == 0)
    {
        spdlog::error("failed to find GPUs with Vulkan support");
        return; // throw std::runtime_error("failed to find GPUs with Vulkan
                // support");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                               physicalDevices.data());

    for (const VkPhysicalDevice& device : physicalDevices)
    {
        if (isDeviceSuitable(device))
        {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
    {
        spdlog::error("failed to find a suitable GPU");
        return; // throw std::runtime_error("failed to find a suitable GPU"");
    }
}

} // namespace vkt