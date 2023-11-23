#include "vulkan_memory_allocator.h"
#include "vulkan_device.h"

namespace jipu
{

VulkanMemoryAllocator::VulkanMemoryAllocator(VulkanDevice* device, const VulkanMemoryAllocatorDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace jipu