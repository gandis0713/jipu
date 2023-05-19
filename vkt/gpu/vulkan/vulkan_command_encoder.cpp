#include "vulkan_command_encoder.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_pipeline.h"

#include <stdexcept>

namespace vkt
{

VulkanCommandEncoder::VulkanCommandEncoder(VulkanCommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor)
    : CommandEncoder(commandBuffer, descriptor)
{
}

void VulkanCommandEncoder::startEncoding()
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);

    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = 0;                  // Optional
    commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    if (vulkanDevice->vkAPI.BeginCommandBuffer(vulkanCommandBuffer->getVkCommandBuffer(), &commandBufferBeginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer.");
    }
}

void VulkanCommandEncoder::endEncoding()
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