#pragma once

#include "vulkan_api.h" // should be included before 'vk_mem_alloc.h'

#include "vk_mem_alloc.h"

namespace jipu
{

struct VulkanMemoryAllocatorDescriptor
{
};

class VulkanDevice;
class VulkanMemoryAllocator final
{
public:
    VulkanMemoryAllocator() = delete;
    VulkanMemoryAllocator(VulkanDevice* device, const VulkanMemoryAllocatorDescriptor& descriptor);
    ~VulkanMemoryAllocator();

private:
    VulkanDevice* m_device;
    VmaAllocator m_allocator = VK_NULL_HANDLE;
    VmaVulkanFunctions m_vmaFunctions;
};

} // namespace jipu