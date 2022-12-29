#include <iostream>
#include <vulkan/vulkan.h>

int main()
{
    // initialize the VkInstanceCreateInfo structure
    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    VkInstance instance;
    VkResult result;

    result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);

    if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        std::cerr << "cannot find a compatible Vulkan ICD" << std::endl;
        return -1;
    }
    else if (result)
    {
        std::cerr << "unknown error" << std::endl;
        return -1;
    }
    // else -> success
    std::cout << "hello vulkan" << std::endl;

    vkDestroyInstance(instance, NULL);
    return 0;
}