#pragma once

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
    ~VulkanCommandBuffer() override = default;

    std::unique_ptr<CommandEncoder> createCommandEncoder(const CommandEncoderDescriptor& descriptor) override;

private:
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
};
} // namespace vkt