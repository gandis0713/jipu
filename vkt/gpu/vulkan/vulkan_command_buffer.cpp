#include "vulkan_command_buffer.h"
#include "vulkan_command_encoder.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, const CommandBufferDescriptor& descriptor)
    : CommandBuffer(device, descriptor)
    , m_commandPool(device->getCommandPool())
{
}

std::unique_ptr<CommandEncoder> VulkanCommandBuffer::createCommandEncoder(const CommandEncoderDescriptor& descriptor)
{
    return std::make_unique<VulkanCommandEncoder>(this, descriptor);
}

} // namespace vkt