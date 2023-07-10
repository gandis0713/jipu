#pragma once

#include "utils/cast.h"
#include "vkt/gpu/command_buffer.h"
#include "vulkan_api.h"

namespace vkt
{

class VulkanDevice;
class VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = delete;
    VulkanCommandBuffer(VulkanDevice* device, const CommandBufferDescriptor& descriptor);
    ~VulkanCommandBuffer() override;

    std::unique_ptr<RenderCommandEncoder> createRenderCommandEncoder(const RenderCommandEncoderDescriptor& descriptor) override;
    std::unique_ptr<BlitCommandEncoder> createBlitCommandEncoder(const BlitCommandEncoderDescriptor& descriptor) override;

    VkCommandBuffer getVkCommandBuffer() const;

private:
    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
};

DOWN_CAST(VulkanCommandBuffer, CommandBuffer);

// Convert Helper
CommandBufferUsage ToCommandBufferUsage(VkCommandBufferUsageFlagBits flag);
VkCommandBufferUsageFlagBits ToVkCommandBufferUsageFlagBits(CommandBufferUsage usage);

} // namespace vkt