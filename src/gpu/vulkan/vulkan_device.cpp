#include "vulkan_device.h"

#include "utils/log.h"
#include "vulkan_adapter.h"

#include <optional>
#include <set>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily; // It should be same with graphics family? ref: https://github.com/google/filament/issues/1532

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }

    static QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice);
};

QueueFamilyIndices QueueFamilyIndices::findQueueFamilies(const VkPhysicalDevice& physicalDevice)
{
    QueueFamilyIndices queueFamilyIndices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    LOG_INFO("queue family property size: {}", queueFamilyProperties.size());
    for (auto i = 0; i < queueFamilyProperties.size(); ++i)
    {
        const auto& queueFamily = queueFamilyProperties[i];
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            queueFamilyIndices.graphicsFamily = static_cast<uint32_t>(i);

            // // TODO: check present family.
            // VkBool32 presentSupport = false;
            // vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

            // if (presentSupport)
            // {
            //     queueFamilyIndices.presentFamily = i;
            // }

            queueFamilyIndices.presentFamily = queueFamilyIndices.graphicsFamily;
        }

        if (queueFamilyIndices.isComplete())
        {
            break;
        }
    }

    return queueFamilyIndices;
}

const std::vector<const char*> getRequiredDeviceExtension()
{
    std::vector<const char*> requiredDeviceExtension;

    requiredDeviceExtension.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    LOG_INFO("Required Device extensions :");
    for (const auto& extension : requiredDeviceExtension)
    {
        LOG_INFO("{}{}", '\t', extension);
    }
    return requiredDeviceExtension;
};

static bool checkDeviceExtensionSupport(const VkPhysicalDevice& physicalDevice)
{
    uint32_t deviceExtensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);

    std::vector<VkExtensionProperties> availableDeviceExtensions(deviceExtensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, availableDeviceExtensions.data());

    const std::vector<const char*> requiredDeviceExtensions = getRequiredDeviceExtension();
    std::set<std::string> requiredDeviceExtensionsTemp(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());

    for (const VkExtensionProperties& availableDeviceExtension : availableDeviceExtensions)
    {
        requiredDeviceExtensionsTemp.erase(availableDeviceExtension.extensionName);
    }

    return requiredDeviceExtensionsTemp.empty();
}

static VkDevice createDevice(VkPhysicalDevice physicalDevice, QueueFamilyIndices queueFamilyIndices)
{
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

    std::vector<const char*> requiredDeviceExtensions = getRequiredDeviceExtension();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

    // TODO: validation layer.
    // // set validation layers to be compatible with older implementations:
    // if (enableValidationLayers)
    // {
    //     const std::vector<const char*>& requiredValidationLayers = getRequiredValidationLayers();
    //     if (enableValidationLayers && !checkValidationLayerSupport(requiredValidationLayers))
    //     {
    //         throw std::runtime_error("validation layers requested, but not "
    //                                  "available for device!");
    //     }
    //     deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(requiredValidationLayers.size());
    //     deviceCreateInfo.ppEnabledLayerNames = requiredValidationLayers.data();
    // }
    // else
    // {
    //     deviceCreateInfo.enabledLayerCount = 0;
    // }

    VkDevice device{};
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    return device;
}

namespace vkt
{

VulkanDevice::VulkanDevice(VulkanAdapter* adapter, DeviceCreateInfo info) : Device(adapter, info)
{
    VkPhysicalDevice physicalDevice = adapter->getPhysicalDevice();

    QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::findQueueFamilies(physicalDevice);
    m_device = createDevice(physicalDevice, queueFamilyIndices);
    if (m_device == nullptr)
    {
        return;
    }

    // create graphics queue
    vkGetDeviceQueue(m_device, queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);

    // create present queue
    vkGetDeviceQueue(m_device, queueFamilyIndices.presentFamily.value(), 0, &m_presentQueue);
}

VkDevice VulkanDevice::getDevice() const { return m_device; }
} // namespace vkt
