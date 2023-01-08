#include "driver.h"
#include "allocation.h"

#include <vector>

namespace vkt
{

Driver::Driver(Platform* platform) : m_platform(*platform)
{
    // TODO: Load Vulkan entry points.

    unsigned int extPropCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extPropCount, nullptr);

    std::vector<VkExtensionProperties> extProps(extPropCount);
    vkEnumerateInstanceExtensionProperties(static_cast<const char*>(nullptr),
                                           &extPropCount, extProps.data());

    bool portabilityEnumerationSupport = false;
    for (const VkExtensionProperties& extProp : extProps)
    {
#if VK_HEADER_VERSION >= 216
        // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance#page_Encountered-VK_ERROR_INCOMPATIBLE_DRIVER
        // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_portability_enumeration.html
        if (!strcmp(extProp.extensionName,
                    VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
            portabilityEnumerationSupport = true;
#endif
    }

    // Create Vulkan instance.
    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

#if VK_HEADER_VERSION >= 216
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance#page_Encountered-VK_ERROR_INCOMPATIBLE_DRIVER
    // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_KHR_portability_enumeration.html
    #if defined(__APPLE__)
    if (portabilityEnumerationSupport)
        instanceCreateInfo.flags |=
            VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif
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

    if (result != VK_SUCCESS)
    {
        // TODO : logging
        return;
    }
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