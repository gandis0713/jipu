#include "vulkan_command_buffer_pool.h"

#include "vulkan_device.h"

#include <spdlog/spdlog.h>

namespace jipu
{

VulkanCommandBufferPool::VulkanCommandBufferPool(VulkanDevice* device)
    : m_device(device)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = 0;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (m_device->vkAPI.CreateCommandPool(m_device->getVkDevice(), &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command pool.");
    }
}

VulkanCommandBufferPool::~VulkanCommandBufferPool()
{
    for (auto& commandBuffer : m_commandBuffers)
    {
        if (commandBuffer.second)
        {
            spdlog::warn("Command buffer is not released in this command buffer pool.");
        }

        m_device->vkAPI.FreeCommandBuffers(m_device->getVkDevice(), m_commandPool, 1, &commandBuffer.first);
    }
}

VkCommandBuffer VulkanCommandBufferPool::create(/* TODO */)
{
    for (auto& commandBuffer : m_commandBuffers)
    {
        if (commandBuffer.second == false)
        {
            commandBuffer.second = true;
            return commandBuffer.first;
        }
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    if (m_device->vkAPI.AllocateCommandBuffers(m_device->getVkDevice(), &commandBufferAllocateInfo, &commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create command buffer.");
    }

    m_commandBuffers.insert(std::make_pair(commandBuffer, true));

    return commandBuffer;
}

} // namespace jipu