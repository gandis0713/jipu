#pragma once

#include "vulkan_api.h"
#include <vector>

namespace vkt
{

enum class SemephoreType
{
    kUndefined = 0,
    kHostToQueue,
    kQueueToHost,
    kQueueToQueue
};

struct SemaphoreDescriptor
{
    SemephoreType type = SemephoreType::kUndefined;
};

class VulkanDevice;
class VulkanSynchronization
{
public:
    VulkanSynchronization() = delete;
    VulkanSynchronization(VulkanDevice* device);
    ~VulkanSynchronization() = default;

    void injectSemaphore(const SemaphoreDescriptor& descriptor, VkSemaphore semaphore);
    [[nodiscard]] std::vector<VkSemaphore> takeoutSemaphore(const SemaphoreDescriptor& descriptor);

private:
    [[maybe_unused]] VulkanDevice* m_device = nullptr;

    struct
    {
        std::vector<VkSemaphore> hostToQueue{};
        std::vector<VkSemaphore> queueToHost{};
        std::vector<VkSemaphore> queueToQueue{};
    } m_semaphores;
};

} // namespace vkt