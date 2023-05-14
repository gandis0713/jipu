#include "vulkan_command_buffer.h"
#include "vulkan_command_encoder.h"

namespace vkt
{

VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandEncoder* encoder, const CommandBufferDescriptor& descriptor)
    : CommandBuffer(encoder, descriptor)
{
}

} // namespace vkt