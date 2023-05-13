#include "vulkan_buffer.h"
#include "vulkan_device.h"
#include "vulkan_physical_device.h"

#include "utils/log.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace vkt
{

VulkanBuffer::VulkanBuffer(VulkanDevice* device, const BufferDescriptor& descriptor) noexcept(false)
    : Buffer(device, descriptor)
{
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = descriptor.size;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    const VulkanAPI& vkAPI = device->vkAPI;
    VkResult result = vkAPI.CreateBuffer(device->getVkDevice(), &bufferCreateInfo, nullptr, &m_buffer);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failed to create buffer. error: {}", result));
    }

    VkMemoryRequirements memoryRequirements{};
    vkAPI.GetBufferMemoryRequirements(device->getVkDevice(), m_buffer, &memoryRequirements);

    spdlog::info("Buffer Memory Requirements");
    spdlog::info("  size: {}", memoryRequirements.size);
    spdlog::info("  alignment: {}", memoryRequirements.alignment);
    spdlog::info("  memoryTypeBits: {}", memoryRequirements.memoryTypeBits);

    VulkanHeapMemoryDescriptor heapMemoryDescriptor{ .flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                     .requirements = memoryRequirements };
    m_heapMemory = std::make_unique<VulkanHeapMemory>(device, heapMemoryDescriptor);

    result = vkAPI.BindBufferMemory(device->getVkDevice(), m_buffer, m_heapMemory->getVkDeviceMemory(), 0);
    if (result != VK_SUCCESS)
    {
        // TODO: delete VkBuffer resource automatically.
        device->vkAPI.DestroyBuffer(device->getVkDevice(), m_buffer, nullptr);
        throw std::runtime_error("Failed to bind memory");
    }
}

VulkanBuffer::~VulkanBuffer()
{
    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyBuffer(vulkanDevice->getVkDevice(), m_buffer, nullptr);
}

void* VulkanBuffer::map()
{
    void* mappedPointer = nullptr;

    auto device = downcast(m_device);
    VkResult result = device->vkAPI.MapMemory(device->getVkDevice(), m_heapMemory->getVkDeviceMemory(), 0, m_size, 0, &mappedPointer);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("Failed to map to pointer. error: {}", result);
    }

    return mappedPointer;
}
void VulkanBuffer::unmap()
{
    auto device = downcast(m_device);
    device->vkAPI.UnmapMemory(device->getVkDevice(), m_heapMemory->getVkDeviceMemory());
}

VkBuffer VulkanBuffer::getVkBuffer() const
{
    return m_buffer;
}

} // namespace vkt