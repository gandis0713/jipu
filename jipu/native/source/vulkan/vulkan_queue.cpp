#include "vulkan_queue.h"

#include "vulkan_device.h"
#include "vulkan_physical_device.h"
#include "vulkan_swapchain.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanQueue::VulkanQueue(VulkanDevice& device, const QueueDescriptor& descriptor) noexcept(false)
    : m_device(device)
{
    VulkanPhysicalDevice& physicalDevice = device.getPhysicalDevice();

    const VulkanPhysicalDeviceInfo& deviceInfo = physicalDevice.getVulkanPhysicalDeviceInfo();

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

    device.vkAPI.GetDeviceQueue(device.getVkDevice(), m_index, 0, &m_queue);

    // create semaphore
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;
    if (device.vkAPI.CreateSemaphore(m_device.getVkDevice(), &semaphoreCreateInfo, nullptr, &m_semaphore) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create semaphore in queue.");
    }

    // create fence.
    m_fence = m_device.getFencePool()->create();
}

VulkanQueue::~VulkanQueue()
{
    auto& vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice.vkAPI;

    // wait idle state before destroy semaphore.
    vkAPI.QueueWaitIdle(m_queue);

    m_device.getFencePool()->release(m_fence);
    vkAPI.DestroySemaphore(vulkanDevice.getVkDevice(), m_semaphore, nullptr);

    // Doesn't need to destroy VkQueue.
}

void VulkanQueue::submit(std::vector<CommandBuffer*> commandBuffers)
{
    std::vector<std::pair<CommandBuffer*, VulkanCommandRecordResult>> commandRecordResults = recordCommands(commandBuffers);

    std::vector<SubmitInfo> submits = generateSubmitInfo(commandRecordResults);

    submit(submits);
}

void VulkanQueue::submit(std::vector<CommandBuffer*> commandBuffers, Swapchain& swapchain)
{
    std::vector<std::pair<CommandBuffer*, VulkanCommandRecordResult>> commandRecordResults = recordCommands(commandBuffers);

    std::vector<SubmitInfo> submits = generateSubmitInfo(commandRecordResults);

    auto& vulkanDevice = downcast(m_device);
    auto& vulkanSwapchain = downcast(swapchain);
    const VulkanAPI& vkAPI = vulkanDevice.vkAPI;

    auto commandBufferCount = commandBuffers.size();
    // we assume the last command buffer is for rendering.
    auto renderCommandBufferIndex = commandBufferCount - 1;

    auto renderCommandBuffer = downcast(commandBuffers[renderCommandBufferIndex]);
    auto acquireImageSemaphore = vulkanSwapchain.getPresentSemaphore();

    // add signal semaphore to signal that render command buffer is finished.
    submits[renderCommandBufferIndex].signal.first.push_back(m_semaphore);

    // add wait semaphore to wait next swapchain image.
    submits[renderCommandBufferIndex].wait.first.push_back(acquireImageSemaphore.first);
    submits[renderCommandBufferIndex].wait.second.push_back(acquireImageSemaphore.second);

    submit(submits);

    swapchain.present(this);
}

VkQueue VulkanQueue::getVkQueue() const
{
    return m_queue;
}

std::vector<VkSemaphore> VulkanQueue::getSemaphores() const
{
    return { m_semaphore };
}

std::vector<std::pair<CommandBuffer*, VulkanCommandRecordResult>> VulkanQueue::recordCommands(std::vector<CommandBuffer*> commandBuffers)
{
    std::vector<std::pair<CommandBuffer*, VulkanCommandRecordResult>> commandRecordResult{};

    for (auto& commandBuffer : commandBuffers)
    {
        auto vulkanCommandBuffer = downcast(commandBuffer);
        auto vulkanDevice = vulkanCommandBuffer->getDevice();

        VulkanCommandRecorderDecsriptor descriptor{};
        descriptor.commandBuffer = vulkanCommandBuffer;

        auto commandBufferRecorder = vulkanDevice->createCommandRecorder(descriptor);
        commandRecordResult.push_back(std::make_pair(commandBuffer, commandBufferRecorder->record()));
    }

    for (const auto& [_, result] : commandRecordResult)
    {
        auto& resourceInfo = result.resourceInfo;

        spdlog::trace("current command buffer buffers src: {}", resourceInfo.src.buffers.size());
        spdlog::trace("current command buffer textures src: {}", resourceInfo.src.textures.size());

        spdlog::trace("current command buffer buffers dst: {}", resourceInfo.dst.buffers.size());
        spdlog::trace("current command buffer textures dst: {}", resourceInfo.dst.textures.size());
    }

    return commandRecordResult;
}

std::vector<VulkanQueue::SubmitInfo> VulkanQueue::generateSubmitInfo(std::vector<std::pair<CommandBuffer*, VulkanCommandRecordResult>> recordResults)
{
    auto& vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice.vkAPI;

    std::vector<SubmitInfo> submitInfo{};

    // newer version
    // if (false)
    {
        auto currentCommandBufferIndex = 0;
        auto findSrcBuffer = [&](Buffer* buffer) -> bool {
            auto begin = recordResults.begin();
            auto end = recordResults.begin() + currentCommandBufferIndex;
            auto it = std::find_if(begin, end, [buffer](const auto& result) {
                return result.second.resourceInfo.src.buffers.contains(buffer);
            });

            return it != end;
        };

        auto findSrcTexture = [&](Texture* texture) -> bool {
            auto begin = recordResults.begin();
            auto end = recordResults.begin() + currentCommandBufferIndex;
            auto it = std::find_if(begin, end, [texture](const auto& result) {
                return result.second.resourceInfo.src.textures.contains(texture);
            });

            return it != end;
        };

        auto getSrcBufferUsageInfo = [&](Buffer* buffer) -> BufferUsageInfo {
            auto begin = recordResults.rbegin() + recordResults.size() - currentCommandBufferIndex;
            auto end = recordResults.rbegin() + 1;
            auto it = std::find_if(begin, end, [buffer](const auto& result) {
                return result.second.resourceInfo.src.buffers.contains(buffer);
            });

            if (it != end)
            {
                return it->second.resourceInfo.src.buffers.at(buffer);
            }

            spdlog::error("Failed to find src buffer usage info.");
            return {};
        };

        auto getSrcTextureUsageInfo = [&](Texture* texture) -> TextureUsageInfo {
            auto begin = recordResults.rbegin() + recordResults.size() - currentCommandBufferIndex;
            auto end = recordResults.rbegin() + 1;
            auto it = std::find_if(begin, end, [texture](const auto& result) {
                return result.second.resourceInfo.src.textures.contains(texture);
            });

            if (it != end)
            {
                return it->second.resourceInfo.src.textures.at(texture);
            }

            spdlog::error("Failed to find src texture usage info.");
            return {};
        };

        for (const auto& [commandBuffer, recordResult] : recordResults)
        {
            SubmitInfo submitInfo{};
            submitInfo.cmdBuf = recordResult.commandBuffer;

            for (const auto& [buffer, dstBufferUsageInfo] : recordResult.resourceInfo.dst.buffers)
            {
                if (findSrcBuffer(buffer))
                {
                    auto srcBufferUsageInfo = getSrcBufferUsageInfo(buffer);
                    // TODO
                }
            }

            for (const auto& [texture, dstTextureUsageInfo] : recordResult.resourceInfo.dst.textures)
            {
                if (findSrcTexture(texture))
                {
                    auto srcTextureUsageInfo = getSrcTextureUsageInfo(texture);
                    // TODO
                }

                if (dstTextureUsageInfo.stageFlags & VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
                {
                    // submitInfo.push_back({commandBuffer, buffer});
                }
            }

            currentCommandBufferIndex++;
        }
    }

    // older version
    if (true)
    {
        submitInfo.resize(recordResults.size());

        int32_t i = 0;
        auto commandBufferSize = recordResults.size();
        for (auto [commandBuffer, commandRecordResult] : recordResults)
        {
            submitInfo[i].cmdBuf = commandRecordResult.commandBuffer;

            int32_t nextIndex = i + 1;
            if (nextIndex < commandBufferSize)
            {
                std::pair<VkSemaphore, VkPipelineStageFlags> signalSemaphore = downcast(commandBuffer)->getSignalSemaphore();
                submitInfo[i].signal.first.push_back(signalSemaphore.first);
                submitInfo[i].signal.second.push_back(signalSemaphore.second);
            }

            int32_t preIndex = i - 1;
            if (preIndex >= 0)
            {
                submitInfo[i].wait.first.push_back(submitInfo[preIndex].signal.first[0]);
                submitInfo[i].wait.second.push_back(submitInfo[preIndex].signal.second[0]);
            }

            auto waitSems = downcast(commandBuffer)->ejectWaitSemaphores();
            for (auto sem : waitSems)
            {
                submitInfo[i].wait.first.push_back(sem.first);
                submitInfo[i].wait.second.push_back(sem.second);
            }

            ++i;
        }
    }

    return submitInfo;
}

void VulkanQueue::submit(const std::vector<SubmitInfo>& submits)
{
    auto& vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice.vkAPI;

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

    auto fence = m_device.getFencePool()->create();
    // m_fencesInFlight.push(std::make_pair(fence, submitInfos));

    VkResult result = vkAPI.QueueSubmit(m_queue, static_cast<uint32_t>(submitInfos.size()), submitInfos.data(), fence);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("failed to submit command buffer {}", static_cast<uint32_t>(result)));
    }

    result = vkAPI.WaitForFences(vulkanDevice.getVkDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("failed to wait for fences {}", static_cast<uint32_t>(result)));
    }

    result = vkAPI.ResetFences(vulkanDevice.getVkDevice(), 1, &fence);
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
