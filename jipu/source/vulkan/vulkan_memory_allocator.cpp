#define VMA_IMPLEMENTATION
#include "vulkan_memory_allocator.h"

#include "vulkan_device.h"
#include "vulkan_driver.h"
#include "vulkan_physical_device.h"

namespace jipu
{

VulkanMemoryAllocator::VulkanMemoryAllocator(VulkanDevice* device, const VulkanMemoryAllocatorDescriptor& descriptor)
    : m_device(device)
{
    auto vulkanPhysicalDevice = downcast(m_device->getPhysicalDevice());
    auto vulkanDriver = downcast(vulkanPhysicalDevice->getDriver());

    auto physicalDevice = vulkanPhysicalDevice->getVkPhysicalDevice();
    auto instance = vulkanDriver->getVkInstance();

#if defined(VMA_DYNAMIC_VULKAN_FUNCTIONS)
    m_vmaFunctions.vkGetInstanceProcAddr = vulkanDriver->vkAPI.GetInstanceProcAddr;
    m_vmaFunctions.vkGetDeviceProcAddr = m_device->vkAPI.GetDeviceProcAddr;
#else
    // TODO: set functions
#endif

    VmaAllocatorCreateInfo createInfo{};
    createInfo.instance = instance;
    createInfo.physicalDevice = physicalDevice;
    createInfo.device = m_device->getVkDevice();
    createInfo.vulkanApiVersion = vulkanDriver->getDriverInfo().apiVersion;
    createInfo.pVulkanFunctions = &m_vmaFunctions;

    VkResult result = vmaCreateAllocator(&createInfo, &m_allocator);
    if (result != VK_SUCCESS)
    {
        std::runtime_error("Failed to create vma allocator");
    }
}

VulkanMemoryAllocator::~VulkanMemoryAllocator()
{
    vmaDestroyAllocator(m_allocator);
}

} // namespace jipu