#include "vulkan_command_buffer.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, const CommandBufferDescriptor& descriptor)
    : CommandBuffer(device, descriptor)
{
}

} // namespace vkt