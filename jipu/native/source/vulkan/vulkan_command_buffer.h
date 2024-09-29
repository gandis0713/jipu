#pragma once

#include "export.h"

#include "common/cast.h"
#include "jipu/command_buffer.h"

#include "vulkan_api.h"
#include "vulkan_command_encoder.h"
#include "vulkan_command_recorder.h"
#include "vulkan_command_resource_synchronizer.h"
#include "vulkan_export.h"

#include <vector>

namespace jipu
{

class VulkanDevice;
class VulkanCommandEncoder;
class VULKAN_EXPORT VulkanCommandBuffer : public CommandBuffer
{
public:
    VulkanCommandBuffer() = delete;
    VulkanCommandBuffer(VulkanCommandEncoder* commandEncoder, const CommandBufferDescriptor& descriptor);
    ~VulkanCommandBuffer() override;

public:
    std::unique_ptr<VulkanCommandRecorder> createCommandRecorder();

public:
    VulkanDevice* getDevice() const;
    VulkanCommandEncoder* getCommandEncoder() const;

public:
    VkCommandBuffer getVkCommandBuffer() const;

    void setSignalSemaphoreStage(VkPipelineStageFlags stage);
    std::pair<VkSemaphore, VkPipelineStageFlags> getSignalSemaphore();

    void injectWaitSemaphore(VkSemaphore semaphore, VkPipelineStageFlags stage);
    std::vector<std::pair<VkSemaphore, VkPipelineStageFlags>> ejectWaitSemaphores();

private:
    VulkanCommandEncoder* m_commandEncoder = nullptr;

private:
    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    VkSemaphore m_signalSemaphore = VK_NULL_HANDLE;
    VkPipelineStageFlags m_signalStage = VK_PIPELINE_STAGE_NONE;

    std::vector<std::pair<VkSemaphore, VkPipelineStageFlags>> m_waitSemaphores{};
};

DOWN_CAST(VulkanCommandBuffer, CommandBuffer);

} // namespace jipu