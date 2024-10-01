#include "vulkan_command_buffer.h"
#include "vulkan_command_encoder.h"
#include "vulkan_device.h"

#include <stdexcept>

namespace jipu
{

VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandEncoder* commandEncoder, const CommandBufferDescriptor& descriptor)
    : m_commandEncoder(commandEncoder)
    , m_commandEncodingResult(m_commandEncoder->result())
{
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    if (m_signalSemaphore)
        getDevice()->vkAPI.DestroySemaphore(getDevice()->getVkDevice(), m_signalSemaphore, nullptr);
}

VulkanDevice* VulkanCommandBuffer::getDevice() const
{
    return m_commandEncoder->getDevice();
}

VulkanCommandEncoder* VulkanCommandBuffer::getCommandEncoder() const
{
    return m_commandEncoder;
}

const CommandEncodingResult& VulkanCommandBuffer::getCommandEncodingResult() const
{
    return m_commandEncodingResult;
}

void VulkanCommandBuffer::setSignalSemaphoreStage(VkPipelineStageFlags stage)
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

        if (getDevice()->vkAPI.CreateSemaphore(getDevice()->getVkDevice(), &semaphoreCreateInfo, nullptr, &m_signalSemaphore) != VK_SUCCESS)
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