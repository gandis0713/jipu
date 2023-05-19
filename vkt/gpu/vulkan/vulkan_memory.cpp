#include "vulkan_memory.h"

#include "vulkan_device.h"
#include "vulkan_physical_device.h"

#include "utils/log.h"

namespace vkt
{

VulkanMemory::VulkanMemory(VulkanDevice* device, const VulkanMemoryDescriptor& descriptor) noexcept(false)
    : m_device(device)
{
    int memoryTypeIndex = downcast(device->getPhysicalDevice())->fineMemoryTypeIndex(descriptor.flags);
    if (memoryTypeIndex == -1)
    {
        throw std::runtime_error("Failed to find memory type index");
    }

    VkMemoryAllocateInfo memoryAllocateInfo{ .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                             .allocationSize = descriptor.requirements.size,
                                             .memoryTypeIndex = static_cast<uint32_t>(memoryTypeIndex) };

    const VulkanAPI& vkAPI = device->vkAPI;
    VkResult result = vkAPI.AllocateMemory(device->getVkDevice(), &memoryAllocateInfo, nullptr, &m_deviceMemory);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate memory");
    }
}

VulkanMemory::~VulkanMemory()
{
    const VulkanAPI& vkAPI = m_device->vkAPI;
    vkAPI.FreeMemory(m_device->getVkDevice(), m_deviceMemory, nullptr);
}

VkDeviceMemory VulkanMemory::getVkDeviceMemory() const
{
    return m_deviceMemory;
}

} // namespace vkt