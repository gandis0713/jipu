#include "vulkan_command_buffer.h"
#include "vulkan_command_encoder.h"
#include "vulkan_device.h"

#include <stdexcept>

namespace jipu
{

VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandEncoder* commandEncoder, const CommandBufferDescriptor& descriptor)
    : m_device(commandEncoder->getDevice())
    , m_commandEncodingContext(std::move(commandEncoder->getEncodingContext()))
    , m_commandPool(m_device->getVkCommandPool())
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1; // create command buffer only one.

    const VulkanAPI& vkAPI = m_device->vkAPI;
    if (vkAPI.AllocateCommandBuffers(m_device->getVkDevice(), &commandBufferAllocateInfo, &m_commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers.");
    }
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    m_device->vkAPI.FreeCommandBuffers(m_device->getVkDevice(), m_commandPool, 1, &m_commandBuffer);
    if (m_signalSemaphore)
        m_device->vkAPI.DestroySemaphore(m_device->getVkDevice(), m_signalSemaphore, nullptr);
}

std::unique_ptr<VulkanCommandRecorder> VulkanCommandBuffer::createCommandRecorder()
{
    return std::make_unique<VulkanCommandRecorder>(this);
}

VulkanDevice& VulkanCommandBuffer::getDevice() const
{
    return *m_device;
}

VkCommandBuffer VulkanCommandBuffer::getVkCommandBuffer() const
{
    return m_commandBuffer;
}

void VulkanCommandBuffer::setSignalPipelineStage(VkPipelineStageFlags stage)
{
    m_signalStage = stage;
}

std::pair<VkSemaphore, VkPipelineStageFlags> VulkanCommandBuffer::getSignalSemaphore()
{
    if (m_signalSemaphore == VK_NULL_HANDLE)
    {
        // create semaphore
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext = nullptr;
        semaphoreCreateInfo.flags = 0;

        if (m_device->vkAPI.CreateSemaphore(m_device->getVkDevice(), &semaphoreCreateInfo, nullptr, &m_signalSemaphore) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render queue semephore.");
        }
    }

    return { m_signalSemaphore, m_signalStage };
}

void VulkanCommandBuffer::injectWaitSemaphore(VkSemaphore semaphore, VkPipelineStageFlags stage)
{
    m_waitSemaphores.emplace_back(semaphore, stage);
}

std::vector<std::pair<VkSemaphore, VkPipelineStageFlags>> VulkanCommandBuffer::ejectWaitSemaphores()
{
    auto waitSemaphores = m_waitSemaphores;

    m_waitSemaphores.clear();

    return waitSemaphores;
}

} // namespace jipu