#include "driver.h"
#include "allocation.h"

namespace vkt
{

Driver::Driver(Platform* platform) : m_platform(*platform)
{
    // TODO: Load Vulkan entry points.

    // Create Vulkan instance.
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#if defined(__APPLE__)
    instanceCreateInfo.flags |=
        VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    instanceCreateInfo.enabledExtensionCount = 0;
    instanceCreateInfo.ppEnabledExtensionNames = nullptr;
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.pNext = nullptr;

    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

    instanceCreateInfo.pApplicationInfo = &applicationInfo;

    VkResult result =
        vkCreateInstance(&instanceCreateInfo, VK_ALLOC_CB, &m_context.instance);

    // Select Physical device.
}

Driver::~Driver() { terminate(); }

void Driver::terminate()
{
    if (m_context.instance == nullptr)
        return;

    vkDestroyInstance(m_context.instance, VK_ALLOC_CB);
    m_context.instance = nullptr;
}

} // namespace vkt