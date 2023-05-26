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

    void injectSemephore(const SemaphoreDescriptor& descriptor, VkSemaphore semaphore);
    std::vector<VkSemaphore> takeoutSemephore(const SemaphoreDescriptor& descriptor);

private:
    VulkanDevice* m_device;

    struct
    {
        std::vector<VkSemaphore> hostToQueue{};
        std::vector<VkSemaphore> queueToHost{};
        std::vector<VkSemaphore> queueToQueue{};
    } m_semaphores;
};

} // namespace vkt