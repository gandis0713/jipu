#include "vulkan_command_buffer.h"
#include "vulkan_command_encoder.h"
#include "vulkan_device.h"

#include <stdexcept>

namespace vkt
{

VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, const CommandBufferDescriptor& descriptor)
    : CommandBuffer(device, descriptor)
    , m_commandPool(device->getVkCommandPool())
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

std::unique_ptr<CommandEncoder> VulkanCommandBuffer::createCommandEncoder(const CommandEncoderDescriptor& descriptor)
{
    return std::make_unique<VulkanCommandEncoder>(this, descriptor);
}

VkCommandBuffer VulkanCommandBuffer::getVkCommandBuffer() const
{
    return m_commandBuffer;
}

CommandBufferUsage ToCommandBufferUsage(VkCommandBufferUsageFlagBits flag)
{
    switch (flag)
    {
    case VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT:
        return CommandBufferUsage::kOneTime;
    default:
        return CommandBufferUsage::kUndefined;
    }
}

VkCommandBufferUsageFlagBits ToVkCommandBufferUsageFlagBits(CommandBufferUsage usage)
{
    switch (usage)
    {
    case CommandBufferUsage::kOneTime:
        return VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    default:
        return static_cast<VkCommandBufferUsageFlagBits>(0x00000000);
    }
}

} // namespace vkt