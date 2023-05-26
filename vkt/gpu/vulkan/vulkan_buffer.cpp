#include "vulkan_buffer.h"
#include "vulkan_device.h"
#include "vulkan_physical_device.h"

#include "utils/log.h"

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

// Convert Helper
VkAccessFlags ToVkAccessFlags(BufferFlags flags)
{
    VkAccessFlags vkFlags = VK_ACCESS_NONE; // 0x00000000

    if (flags & BufferFlagBits::kMapRead)
    {
        vkFlags |= VK_ACCESS_HOST_READ_BIT;
    }
    if (flags & BufferFlagBits::kMapWrite)
    {
        vkFlags |= VK_ACCESS_HOST_WRITE_BIT;
    }
    if (flags & BufferFlagBits::kCopySrc)
    {
        vkFlags |= VK_ACCESS_TRANSFER_READ_BIT;
    }
    if (flags & BufferFlagBits::kCopyDst)
    {
        vkFlags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (flags & BufferFlagBits::kIndex)
    {
        vkFlags |= VK_ACCESS_INDEX_READ_BIT;
    }
    if (flags & BufferFlagBits::kVertex)
    {
        vkFlags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    }
    if (flags & BufferFlagBits::kUniform)
    {
        vkFlags |= VK_ACCESS_UNIFORM_READ_BIT;
    }

    return vkFlags;
}

BufferFlags ToBufferFlags(VkAccessFlags vkflags)
{
    BufferFlags flags = BufferFlagBits::kInvalid; // 0x00000000

    if (vkflags & VK_ACCESS_HOST_READ_BIT)
    {
        flags |= BufferFlagBits::kMapRead;
    }
    if (vkflags & VK_ACCESS_HOST_WRITE_BIT)
    {
        flags |= BufferFlagBits::kMapWrite;
    }
    if (vkflags & VK_ACCESS_TRANSFER_READ_BIT)
    {
        flags |= BufferFlagBits::kCopySrc;
    }
    if (vkflags & VK_ACCESS_TRANSFER_WRITE_BIT)
    {
        flags |= BufferFlagBits::kCopyDst;
    }
    if (vkflags & VK_ACCESS_INDEX_READ_BIT)
    {
        flags = BufferFlagBits::kIndex | BufferFlagBits::kCopyDst;
    }
    if (vkflags & VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)
    {
        flags = flags | BufferFlagBits::kVertex;
    }
    if (vkflags & VK_ACCESS_UNIFORM_READ_BIT)
    {
        flags |= BufferFlagBits::kUniform;
    }

    return flags;
}
} // namespace vkt