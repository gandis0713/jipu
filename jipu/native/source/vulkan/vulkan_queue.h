#pragma once

#include "common/cast.h"
#include "jipu/queue.h"
#include "vulkan_api.h"
#include "vulkan_command_recorder.h"
#include "vulkan_export.h"
#include "vulkan_inflight_context.h"
#include "vulkan_swapchain.h"

#include <unordered_map>

namespace jipu
{

class VulkanDevice;
class VULKAN_EXPORT VulkanQueue : public Queue
{
public:
    VulkanQueue() = delete;
    VulkanQueue(VulkanDevice& device, const QueueDescriptor& descriptor) noexcept(false);
    ~VulkanQueue() override;

    void submit(std::vector<CommandBuffer*> commandBuffers) override;
    void submit(std::vector<CommandBuffer*> commandBuffers, Swapchain& swapchain) override;

public:
    void present(VulkanPresentInfo presentInfo);

private:
    VkQueue getVkQueue(uint32_t index = 0) const;
    VkQueue getVkQueue(QueueFlags flags = QueueFlagBits::kAll) const;

private:
    VulkanDevice& m_device;

private:
    VkFence m_fence = VK_NULL_HANDLE;

    std::vector<std::pair<VkQueue, QueueFlags>> m_queues{};

private:
    std::vector<VulkanSubmit::Info> m_presentSubmitInfos{};

    std::vector<VulkanCommandRecordResult> recordCommands(std::vector<CommandBuffer*> commandBuffers);
    void submit(const std::vector<VulkanSubmit::Info>& submitInfos);
};

DOWN_CAST(VulkanQueue, Queue);

// Convert Helper
VkQueueFlags ToVkQueueFlags(QueueFlags flags);
QueueFlags ToQueueFlags(VkQueueFlags vkflags);

} // namespace jipu