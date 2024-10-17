#pragma once

#include "common/cast.h"
#include "jipu/queue.h"
#include "vulkan_api.h"
#include "vulkan_command_recorder.h"
#include "vulkan_export.h"
#include "vulkan_inflight_context.h"
#include "vulkan_swapchain.h"

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

public:
    VkQueue getVkQueue() const;
    std::vector<VkSemaphore> getSemaphores() const;

protected:
    VulkanDevice& m_device;

private:
    VkQueue m_queue = VK_NULL_HANDLE;
    VkFence m_fence = VK_NULL_HANDLE;
    VkSemaphore m_semaphore = VK_NULL_HANDLE;

    // TODO: use pair.
    uint32_t m_index{ 0 }; // Index in VkQueueFamilyProperties in VkPhysicalDevice
    VkQueueFamilyProperties m_properties{};

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