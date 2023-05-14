#include "vulkan_buffer.h"
#include "vulkan_device.h"
#include "vulkan_physical_device.h"

#include "utils/log.h"

#include <stdexcept>

namespace vkt
{

VkAccessFlags BufferUsage2VkAccessFlags(BufferUsage usage)
{
    VkAccessFlags flags = VK_ACCESS_NONE; // 0x00000000

    if (any(usage & BufferUsage::kMapRead))
    {
        flags |= VK_ACCESS_HOST_READ_BIT;
    }
    if (any(usage & BufferUsage::kMapWrite))
    {
        flags |= VK_ACCESS_HOST_WRITE_BIT;
    }
    if (any(usage & BufferUsage::kCopySrc))
    {
        flags |= VK_ACCESS_TRANSFER_READ_BIT;
    }
    if (any(usage & BufferUsage::kCopyDst))
    {
        flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (any(usage & BufferUsage::kIndex))
    {
        flags |= VK_ACCESS_INDEX_READ_BIT;
    }
    if (any(usage & BufferUsage::kVertex))
    {
        flags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    }
    if (any(usage & BufferUsage::kUniform))
    {
        flags |= VK_ACCESS_UNIFORM_READ_BIT;
    }

    return flags;
}

BufferUsage VkAccessFlags2BufferUsage(VkAccessFlags flags)
{
    BufferUsage usage = BufferUsage::kInvalid; // 0x00000000

    if (flags & VK_ACCESS_HOST_READ_BIT)
    {
        usage |= BufferUsage::kMapRead;
    }
    if (flags & VK_ACCESS_HOST_WRITE_BIT)
    {
        usage |= BufferUsage::kMapWrite;
    }
    if (flags & VK_ACCESS_TRANSFER_READ_BIT)
    {
        usage |= BufferUsage::kCopySrc;
    }
    if (flags & VK_ACCESS_TRANSFER_WRITE_BIT)
    {
        usage |= BufferUsage::kCopyDst;
    }
    if (flags & VK_ACCESS_INDEX_READ_BIT)
    {
        usage = BufferUsage::kIndex | BufferUsage::kCopyDst;
    }
    if (flags & VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)
    {
        usage = usage | BufferUsage::kVertex;
    }
    if (flags & VK_ACCESS_UNIFORM_READ_BIT)
    {
        usage |= BufferUsage::kUniform;
    }

    return usage;
}

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

    VulkanMemoryDescriptor heapMemoryDescriptor{ .flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                 .requirements = memoryRequirements };
    m_memory = std::make_unique<VulkanMemory>(device, heapMemoryDescriptor);

    result = vkAPI.BindBufferMemory(device->getVkDevice(), m_buffer, m_memory->getVkDeviceMemory(), 0);
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
    VkResult result = device->vkAPI.MapMemory(device->getVkDevice(), m_memory->getVkDeviceMemory(), 0, m_size, 0, &mappedPointer);
    if (result != VK_SUCCESS)
    {
        LOG_ERROR("Failed to map to pointer. error: {}", result);
    }

    return mappedPointer;
}
void VulkanBuffer::unmap()
{
    auto device = downcast(m_device);
    device->vkAPI.UnmapMemory(device->getVkDevice(), m_memory->getVkDeviceMemory());
}

VkBuffer VulkanBuffer::getVkBuffer() const
{
    return m_buffer;
}

} // namespace vkt