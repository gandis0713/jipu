#pragma once

#include "vulkan_api.h" // should be included before 'vk_mem_alloc.h'

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
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
    VulkanDevice* m_device = nullptr;
    VmaAllocator m_allocator = VK_NULL_HANDLE;
    VmaVulkanFunctions m_vmaFunctions{};
};

} // namespace jipu