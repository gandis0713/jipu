#include "vulkan_queue.h"
#include "vulkan_device.h"
#include "vulkan_physical_device.h"
#include "vulkan_swapchain.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanQueue::VulkanQueue(VulkanDevice* device, const QueueDescriptor& descriptor) noexcept(false)
    : Queue(device, descriptor)
{
    VulkanPhysicalDevice* physicalDevice = device->getPhysicalDevice();

    const VulkanPhysicalDeviceInfo& deviceInfo = physicalDevice->getVulkanPhysicalDeviceInfo();

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
    std::vector<SubmitInfo> submits = gatherSubmitInfo(commandBuffers);

    submit(submits);
}

void VulkanQueue::submit(std::vector<CommandBuffer*> commandBuffers, Swapchain* swapchain)
{
    std::vector<SubmitInfo> submits = gatherSubmitInfo(commandBuffers);

    auto vulkanDevice = downcast(m_device);
    auto vulkanSwapchain = downcast(swapchain);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    auto commandBufferCount = commandBuffers.size();
    // we assume the last command buffer is for rendering.
    auto renderCommandBufferIndex = commandBufferCount - 1;

    auto renderCommandBuffer = downcast(commandBuffers[renderCommandBufferIndex]);
    auto acquireImageSemaphore = vulkanSwapchain->getPresentSemaphore();
    auto renderSemaphore = vulkanSwapchain->getRenderSemaphore();

    // add signal semaphore to signal that render command buffer is finished.
    submits[renderCommandBufferIndex].signal.first.push_back(renderSemaphore.first);

    // add wait semaphore to wait next swapchain image.
    submits[renderCommandBufferIndex].wait.first.push_back(acquireImageSemaphore.first);
    submits[renderCommandBufferIndex].wait.second.push_back(acquireImageSemaphore.second);

    submit(submits);

    swapchain->present(this);
}

VkQueue VulkanQueue::getVkQueue() const
{
    return m_queue;
}

std::vector<VulkanQueue::SubmitInfo> VulkanQueue::gatherSubmitInfo(std::vector<CommandBuffer*> commandBuffers)
{
    auto vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    std::vector<SubmitInfo> submitInfo{};

    auto commandBufferSize = commandBuffers.size();
    submitInfo.resize(commandBufferSize);

    for (auto i = 0; i < commandBufferSize; ++i)
    {
        submitInfo[i].cmdBuf = downcast(commandBuffers[i])->getVkCommandBuffer();

        auto nextIndex = i + 1;
        if (nextIndex < commandBufferSize)
        {
            auto signalSemaphore = downcast(commandBuffers[i])->getSignalSemaphore();
            submitInfo[i].signal.first.push_back(signalSemaphore.first);
            submitInfo[i].signal.second.push_back(signalSemaphore.second);
        }

        auto preIndex = i - 1;
        if (preIndex >= 0)
        {
            submitInfo[i].wait.first.push_back(submitInfo[preIndex].signal.first[0]);
            submitInfo[i].wait.second.push_back(submitInfo[preIndex].signal.second[0]);
        }

        auto waitSems = downcast(commandBuffers[i])->ejectWaitSemaphores();
        for (auto sem : waitSems)
        {
            submitInfo[i].wait.first.push_back(sem.first);
            submitInfo[i].wait.second.push_back(sem.second);
        }
    }

    return submitInfo;
}

void VulkanQueue::submit(const std::vector<SubmitInfo>& submits)
{
    auto vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    auto submitInfoSize = submits.size();

    std::vector<VkSubmitInfo> submitInfos{};
    submitInfos.resize(submitInfoSize);

    for (auto i = 0; i < submitInfoSize; ++i)
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &submits[i].cmdBuf;

        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(submits[i].signal.first.size());
        submitInfo.pSignalSemaphores = submits[i].signal.first.data();

        submitInfo.pWaitSemaphores = submits[i].wait.first.data();
        submitInfo.pWaitDstStageMask = submits[i].wait.second.data();
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(submits[i].wait.first.size());

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

} // namespace jipu
