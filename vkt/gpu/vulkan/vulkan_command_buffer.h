#pragma once

#include "vkt/gpu/command_buffer.h"
#include "vulkan_api.h"

#include "export.h"
#include "utils/cast.h"

namespace vkt
{

class VulkanDevice;
class VKT_EXPORT VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = delete;
    VulkanCommandBuffer(VulkanDevice* device, const CommandBufferDescriptor& descriptor);
    ~VulkanCommandBuffer() override;

    std::unique_ptr<RenderCommandEncoder> createRenderCommandEncoder(const RenderCommandEncoderDescriptor& descriptor) override;

    VkCommandBuffer getVkCommandBuffer() const;

private:
    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
};

DOWN_CAST(VulkanCommandBuffer, CommandBuffer);

} // namespace vkt