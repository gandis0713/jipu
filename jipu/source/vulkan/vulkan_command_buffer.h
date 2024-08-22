#pragma once

#include "export.h"

#include "jipu/command_buffer.h"
#include "utils/cast.h"
#include "vulkan_api.h"
#include "vulkan_export.h"

namespace jipu
{

class VulkanDevice;
class VULKAN_EXPORT VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = delete;
    VulkanCommandBuffer(VulkanDevice& device, const CommandBufferDescriptor& descriptor);
    ~VulkanCommandBuffer() override;

    std::unique_ptr<CommandEncoder> createCommandEncoder(const CommandEncoderDescriptor& descriptor) override;

public:
    VulkanDevice& getDevice() const;

public:
    VkCommandBuffer getVkCommandBuffer() const;

    void setSignalPipelineStage(VkPipelineStageFlags stage);
    std::pair<VkSemaphore, VkPipelineStageFlags> getSignalSemaphore();

    void injectWaitSemaphore(VkSemaphore semaphore, VkPipelineStageFlags stage);
    std::vector<std::pair<VkSemaphore, VkPipelineStageFlags>> ejectWaitSemaphores();

private:
    VulkanDevice& m_device;

private:
    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    VkSemaphore m_signalSemaphore = VK_NULL_HANDLE;
    VkPipelineStageFlags m_signalStage = VK_PIPELINE_STAGE_NONE;

    std::vector<std::pair<VkSemaphore, VkPipelineStageFlags>> m_waitSemaphores{};
};

DOWN_CAST(VulkanCommandBuffer, CommandBuffer);

} // namespace jipu