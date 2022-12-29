#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

VkInstance instance;
VkPhysicalDevice physical_device;

VkResult checkInstanceLayer();
VkResult createInstance();
VkResult createPhysicalDevice();

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
        std::cout << "Layer Name             : " << p.layerName << '\n'
                  << "Spec Version           : " << p.specVersion << '\n'
                  << "Implementation Version : " << p.implementationVersion
                  << '\n'
                  << "Description            : " << p.description << '\n'
                  << std::endl;
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

    return result;
}