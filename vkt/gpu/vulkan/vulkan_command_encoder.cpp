#include "vulkan_command_encoder.h"
#include "vulkan_command_buffer.h"

namespace vkt
{

VulkanCommandEncoder::VulkanCommandEncoder(VulkanCommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor)
    : CommandEncoder(commandBuffer, descriptor)
{
}

} // namespace vkt