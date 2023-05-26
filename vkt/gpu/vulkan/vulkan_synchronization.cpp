#include "vulkan_synchronization.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanSynchronization::VulkanSynchronization(VulkanDevice* device)
    : m_device(device)
{
}

void VulkanSynchronization::injectSemephore(const SemaphoreDescriptor& descriptor, VkSemaphore semaphore)
{
    switch (descriptor.type)
    {
    case SemephoreType::kHostToQueue:
        m_semaphores.hostToQueue.push_back(semaphore);
        break;
    case SemephoreType::kQueueToHost:
        m_semaphores.queueToHost.push_back(semaphore);
        break;
    case SemephoreType::kQueueToQueue:
        m_semaphores.queueToQueue.push_back(semaphore);
        break;
    default:
        // do nothing.
        break;
    }
}

std::vector<VkSemaphore> VulkanSynchronization::takeoutSemephore(const SemaphoreDescriptor& descriptor)
{
    std::vector<VkSemaphore> semaphores;
    switch (descriptor.type)
    {
    case SemephoreType::kHostToQueue:
        semaphores = m_semaphores.hostToQueue;
        m_semaphores.hostToQueue.clear();
        break;

    case SemephoreType::kQueueToHost:
        semaphores = m_semaphores.queueToHost;
        m_semaphores.queueToHost.clear();
        break;

    case SemephoreType::kQueueToQueue:
        semaphores = m_semaphores.queueToQueue;
        m_semaphores.queueToQueue.clear();
        break;

    default:
        // do nothing
        break;
    }

    return semaphores;
}

} // namespace vkt