#include "vulkan_buffer.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanBuffer::VulkanBuffer(VulkanDevice* device, const BufferDescriptor& descriptor)
    : Buffer(device, descriptor)
{
}

} // namespace vkt