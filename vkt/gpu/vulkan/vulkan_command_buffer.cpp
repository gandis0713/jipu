#include "vulkan_command_buffer.h"
#include "vulkan_command_encoder.h"
#include "vulkan_device.h"

#include <stdexcept>

namespace vkt
{

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, const CommandBufferDescriptor& descriptor)
    : CommandBuffer(device, descriptor)
    , m_commandPool(device->getCommandPool())
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1; // create command buffer only one.

    const VulkanAPI& vkAPI = device->vkAPI;
    if (vkAPI.AllocateCommandBuffers(device->getVkDevice(), &commandBufferAllocateInfo, &m_commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers.");
    }
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    auto vulkanDevice = downcast(m_device);

    vulkanDevice->vkAPI.FreeCommandBuffers(vulkanDevice->getVkDevice(), m_commandPool, 1, &m_commandBuffer);
}

std::unique_ptr<RenderCommandEncoder> VulkanCommandBuffer::createRenderCommandEncoder(const RenderCommandEncoderDescriptor& descriptor)
{
    return std::make_unique<VulkanRenderCommandEncoder>(this, descriptor);
}

VkCommandBuffer VulkanCommandBuffer::getVkCommandBuffer() const
{
    return m_commandBuffer;
}

} // namespace vkt