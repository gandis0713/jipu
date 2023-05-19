#include "vulkan_command_encoder.h"
#include "vulkan_command_buffer.h"

namespace vkt
{

VulkanCommandEncoder::VulkanCommandEncoder(VulkanCommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor)
    : CommandEncoder(commandBuffer, descriptor)
{
}

void VulkanCommandEncoder::setPipeline(Pipeline* pipeline)
{
}

void VulkanCommandEncoder::setVertexBuffer(Buffer* buffer)
{
}

void VulkanCommandEncoder::setIndexBuffer(Buffer* buffer)
{
}

} // namespace vkt