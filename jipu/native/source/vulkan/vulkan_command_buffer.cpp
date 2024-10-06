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
    releaseVkCommandBuffer();

    auto device = getDevice();
    if (m_signalSemaphore)
        device->vkAPI.DestroySemaphore(device->getVkDevice(), m_signalSemaphore, nullptr);
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

VulkanCommandRecordResult VulkanCommandBuffer::recordToVkCommandBuffer()
{
    releaseVkCommandBuffer();
    createVkCommandBuffer();

    auto commandRecorder = createCommandRecorder();

    return commandRecorder->record();
}

VkCommandBuffer VulkanCommandBuffer::getVkCommandBuffer()
{
    return m_commandBuffer;
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

std::unique_ptr<VulkanCommandRecorder> VulkanCommandBuffer::createCommandRecorder()
{
    return std::make_unique<VulkanCommandRecorder>(this);
}

void VulkanCommandBuffer::createVkCommandBuffer()
{
    releaseVkCommandBuffer();

    m_commandBuffer = getDevice()->getCommandPool()->create();
}

void VulkanCommandBuffer::releaseVkCommandBuffer()
{
    if (m_commandBuffer)
    {
        getDevice()->getCommandPool()->release(m_commandBuffer);
        m_commandBuffer = VK_NULL_HANDLE;
    }
}

} // namespace jipu