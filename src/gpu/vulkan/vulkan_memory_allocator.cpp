#include "vulkan_memory_allocator.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanMemoryAllocator::VulkanMemoryAllocator(VulkanDevice* device, const VulkanMemoryAllocatorDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace vkt