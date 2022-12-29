#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

VkInstance instance;
VkPhysicalDevice physical_device;
VkDevice device;
VkQueue graphicsQueue;

constexpr float graphicsQueuePriority = 1.0f;

uint32_t graphicsQueueFamilyIndex = 0xffff;

VkResult checkInstanceLayer();
VkResult createInstance();
VkResult createPhysicalDevice();
VkResult createDevice();

int main()
{
    VkResult result = VK_SUCCESS;

    // check instance layer.
    result = checkInstanceLayer();
    if (result != VK_SUCCESS)
    {
        return -1;
    }

    // create instance.
    result = createInstance();
    if (result != VK_SUCCESS)
    {
        return -1;
    }

    // create physical device
    result = createPhysicalDevice();
    if (result != VK_SUCCESS)
    {
        return -1;
    }

    // create device
    result = createDevice();
    if (result != VK_SUCCESS)
    {
        return -1;
    }

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);

    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);

    return 0;
}

VkResult checkInstanceLayer()
{
    uint32_t layerCount{ 0 };
    VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to get layer count [Error code : " << result << "]"
                  << std::endl;
        return result;
    }

    std::vector<VkLayerProperties> layerProperties;
    layerProperties.resize(static_cast<std::size_t>(layerCount));

    result =
        vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to get layer properties [Error code : " << result
                  << "]" << std::endl;
        return result;
    }

    for (const VkLayerProperties& p : layerProperties)
    {
        std::cout << "Layer Name             : " << p.layerName << std::endl
                  << "Spec Version           : " << p.specVersion << std::endl
                  << "Implementation Version : " << p.implementationVersion
                  << std::endl
                  << "Description            : " << p.description << std::endl;
        std::cout << std::endl;
    }

    return result;
}

VkResult createInstance()
{
    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create instance [Error code : " << result << "]"
                  << std::endl;
        return result;
    }

    return result;
}

VkResult createPhysicalDevice()
{
    uint32_t physicalDeviceCount{ 0 };
    VkResult result =
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to get physical device count [Error code : "
                  << result << "]" << std::endl;
        return result;
    }

    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(static_cast<std::size_t>(physicalDeviceCount));

    result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                                        physicalDevices.data());
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to get physical devices [Error code : " << result
                  << "]" << std::endl;
        return result;
    }

    for (const VkPhysicalDevice& physicalDevice : physicalDevices)
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;

        vkGetPhysicalDeviceProperties(physicalDevice,
                                      &physicalDeviceProperties);

        std::cout << physicalDeviceProperties.deviceName << std::endl;
    }

    physical_device = physicalDevices[0];

    return result;
}

VkResult createDevice()
{
    VkResult result = VK_SUCCESS;

    uint32_t queueFamilyPropertyCount{ 0 };

    vkGetPhysicalDeviceQueueFamilyProperties(
        physical_device, &queueFamilyPropertyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    queueFamilyProperties.resize(queueFamilyPropertyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                             &queueFamilyPropertyCount,
                                             queueFamilyProperties.data());

    bool graphicsQueueFlag = false;
    bool computeQueueFlag = false;
    bool transferQueueFlag = false;
    bool sparseQueueFlag = false;
    for (uint32_t index = 0; index < queueFamilyProperties.size(); ++index)
    {
        std::cout << std::endl;

        graphicsQueueFlag =
            (queueFamilyProperties[index].queueFlags & VK_QUEUE_GRAPHICS_BIT);
        computeQueueFlag =
            (queueFamilyProperties[index].queueFlags & VK_QUEUE_COMPUTE_BIT);
        transferQueueFlag =
            (queueFamilyProperties[index].queueFlags & VK_QUEUE_TRANSFER_BIT);
        sparseQueueFlag = (queueFamilyProperties[index].queueFlags &
                           VK_QUEUE_SPARSE_BINDING_BIT);

        std::cout << std::boolalpha
                  << "graphicsQueueFlag : " << graphicsQueueFlag << std::endl;
        std::cout << std::boolalpha << "computeQueueFlag : " << computeQueueFlag
                  << std::endl;
        std::cout << std::boolalpha
                  << "transferQueueFlag : " << transferQueueFlag << std::endl;
        std::cout << std::boolalpha << "sparseQueueFlag : " << sparseQueueFlag
                  << std::endl;

        std::cout << "queueCount : " << queueFamilyProperties[index].queueCount
                  << std::endl;

        if (queueFamilyProperties[index].queueCount < 1)
        {
            continue;
        }

        if (graphicsQueueFlag)
        {
            graphicsQueueFamilyIndex = index;
            break;
        }
    }

    if (graphicsQueueFamilyIndex == 0xffff)
    {
        std::cerr << "There is no queue family for graphics" << std::endl;
        return VK_ERROR_UNKNOWN;
    }

    VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = &graphicsQueuePriority;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;

    result =
        vkCreateDevice(physical_device, &deviceCreateInfo, nullptr, &device);
    if (result != VK_SUCCESS)
    {
        std::cerr << "Failed to create device." << std::endl;
    }

    return result;
}