#include "vulkan_queue.h"
#include "vulkan_device.h"
#include "vulkan_physical_device.h"
#include "vulkan_swapchain.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace vkt
{

VulkanQueue::VulkanQueue(VulkanDevice* device, const QueueDescriptor& descriptor) noexcept(false)
    : Queue(device, descriptor)
{
    VulkanPhysicalDevice* physicalDevice = downcast(m_device->getPhysicalDevice());

    const VulkanPhysicalDeviceInfo& deviceInfo = physicalDevice->getInfo();

    const uint64_t queueFamilyPropertiesSize = deviceInfo.queueFamilyProperties.size();
    if (queueFamilyPropertiesSize <= 0)
    {
        throw std::runtime_error("There is no queue family properties.");
    }

    for (uint64_t index = 0; index < queueFamilyPropertiesSize; ++index)
    {
        const auto& properties = deviceInfo.queueFamilyProperties[index];
        if (properties.queueFlags & ToVkQueueFlags(descriptor.flags))
        {
            m_index = static_cast<uint32_t>(index);
            m_properties = properties;
            break;
        }
    }

    device->vkAPI.GetDeviceQueue(device->getVkDevice(), m_index, 0, &m_queue);

    // create fence.
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = 0;
    // fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (device->vkAPI.CreateFence(device->getVkDevice(), &fenceCreateInfo, nullptr, &m_fence) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create render queue fence.");
    }
}

VulkanQueue::~VulkanQueue()
{
    auto vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    // wait idle state before destroy semaphore.
    vkAPI.QueueWaitIdle(m_queue);

    // Doesn't need to destroy VkQueue.
    vkAPI.DestroyFence(vulkanDevice->getVkDevice(), m_fence, nullptr);
}

void VulkanQueue::submit(std::vector<CommandBuffer*> commandBuffers)
{
    auto vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    std::vector<VkCommandBuffer> buffers{};
    std::vector<VkSubmitInfo> submitInfos{};
    std::vector<std::pair<VkSemaphore, VkPipelineStageFlags>> signalSemaphores{};
    std::vector<std::pair<std::vector<VkSemaphore>, std::vector<VkPipelineStageFlags>>> waitSemaphores{};

    auto commandBufferSize = commandBuffers.size();
    buffers.resize(commandBufferSize);
    submitInfos.resize(commandBufferSize);
    signalSemaphores.resize(commandBufferSize);
    waitSemaphores.resize(commandBufferSize);

    for (auto i = 0; i < commandBufferSize; ++i)
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        buffers[i] = downcast(commandBuffers[i])->getVkCommandBuffer();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buffers[i];

        // send signal even if last command buffer.
        signalSemaphores[i] = downcast(commandBuffers[i])->getSignalSemaphore();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signalSemaphores[i].first;

        auto preIndex = i - 1;
        if (preIndex >= 0)
        {
            waitSemaphores[i].first.push_back(signalSemaphores[preIndex].first);
            waitSemaphores[i].second.push_back(signalSemaphores[preIndex].second);
        }

        auto waitSems = downcast(commandBuffers[i])->ejectWaitSemaphores();
        for (auto sem : waitSems)
        {
            waitSemaphores[i].first.push_back(sem.first);
            waitSemaphores[i].second.push_back(sem.second);
        }

        submitInfo.pWaitSemaphores = waitSemaphores[i].first.data();
        submitInfo.pWaitDstStageMask = waitSemaphores[i].second.data();
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores[i].first.size());

        submitInfos[i] = submitInfo;
    }

    VkResult result = vkAPI.QueueSubmit(m_queue, static_cast<uint32_t>(submitInfos.size()), submitInfos.data(), m_fence);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("failed to submit command buffer {}", static_cast<uint32_t>(result)));
    }

    result = vkAPI.WaitForFences(vulkanDevice->getVkDevice(), 1, &m_fence, VK_TRUE, UINT64_MAX);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("failed to wait for fences {}", static_cast<uint32_t>(result)));
    }

    result = vkAPI.ResetFences(vulkanDevice->getVkDevice(), 1, &m_fence);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("failed to reset for fences {}", static_cast<uint32_t>(result)));
    }
}

void VulkanQueue::submit(std::vector<CommandBuffer*> commandBuffers, Swapchain* swapchain)
{
    auto vulkanDevice = downcast(m_device);
    auto vulkanSwapchain = downcast(swapchain);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    auto commandBufferCount = commandBuffers.size();
    auto renderCommandBuffer = downcast(commandBuffers[commandBufferCount - 1]);
    auto acquireImageSemaphore = vulkanSwapchain->getSignalSemaphore();
    renderCommandBuffer->injectWaitSemaphore(acquireImageSemaphore.first, acquireImageSemaphore.second);
    vulkanSwapchain->injectSignalSemaphore(renderCommandBuffer->getSignalSemaphore().first);

    submit(commandBuffers);

    swapchain->present(this);
}

VkQueue VulkanQueue::getVkQueue() const
{
    return m_queue;
}

// Convert Helper
VkQueueFlags ToVkQueueFlags(QueueFlags flags)
{
    VkQueueFlags vkflags = 0u;

    if (flags & QueueFlagBits::kGraphics)
    {
        vkflags |= VK_QUEUE_GRAPHICS_BIT;
    }
    if (flags & QueueFlagBits::kCompute)
    {
        vkflags |= VK_QUEUE_COMPUTE_BIT;
    }
    if (flags & QueueFlagBits::kTransfer)
    {
        vkflags |= VK_QUEUE_TRANSFER_BIT;
    }

    return vkflags;
}

QueueFlags ToQueueFlags(VkQueueFlags vkflags)
{
    QueueFlags flags = 0u;

    if (vkflags & VK_QUEUE_GRAPHICS_BIT)
    {
        flags |= QueueFlagBits::kGraphics;
    }
    if (vkflags & VK_QUEUE_COMPUTE_BIT)
    {
        flags |= QueueFlagBits::kCompute;
    }
    if (vkflags & VK_QUEUE_TRANSFER_BIT)
    {
        flags |= QueueFlagBits::kTransfer;
    }

    return flags;
}

} // namespace vkt
