#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_physical_device.h"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanBuffer::VulkanBuffer(VulkanDevice* device, const BufferDescriptor& descriptor) noexcept(false)
    : Buffer(device, descriptor)
{
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = descriptor.size;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.usage = ToVkBufferUsageFlags(descriptor.usage);
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    const VulkanAPI& vkAPI = device->vkAPI;
    VkResult result = vkAPI.CreateBuffer(device->getVkDevice(), &bufferCreateInfo, nullptr, &m_buffer);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failed to create buffer. error: {}", static_cast<int32_t>(result)));
    }

    VkMemoryRequirements memoryRequirements{};
    vkAPI.GetBufferMemoryRequirements(device->getVkDevice(), m_buffer, &memoryRequirements);

    // TODO: memory optimization
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
    unmap();

    m_memory.reset();

    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyBuffer(vulkanDevice->getVkDevice(), m_buffer, nullptr);
}

void* VulkanBuffer::map()
{
    if (m_mappedPtr == nullptr)
    {
        auto device = downcast(m_device);
        VkResult result = device->vkAPI.MapMemory(device->getVkDevice(), m_memory->getVkDeviceMemory(), 0, m_descriptor.size, 0, &m_mappedPtr);
        if (result != VK_SUCCESS)
        {
            spdlog::error("Failed to map to pointer. error: {}", static_cast<int32_t>(result));
        }
    }

    return m_mappedPtr;
}
void VulkanBuffer::unmap()
{
    if (m_mappedPtr)
    {
        auto device = downcast(m_device);
        device->vkAPI.UnmapMemory(device->getVkDevice(), m_memory->getVkDeviceMemory());

        m_mappedPtr = nullptr;
    }
}

void VulkanBuffer::setTransition(CommandBuffer* commandBuffer, VkPipelineStageFlags flags)
{
    auto vulkanDevice = downcast(m_device);
    auto vulkanCommandBuffer = downcast(commandBuffer);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    VkBufferMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.srcAccessMask = ToVkAccessFlags(m_descriptor.usage);
    barrier.dstAccessMask = ToVkAccessFlags(m_descriptor.usage);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.buffer = m_buffer;
    barrier.size = getSize();
    barrier.offset = 0;

    vkAPI.CmdPipelineBarrier(vulkanCommandBuffer->getVkCommandBuffer(),
                             m_stageFlags, flags,
                             0,
                             0, nullptr,
                             1, &barrier,
                             0, nullptr);

    m_stageFlags = flags;
}

VkBuffer VulkanBuffer::getVkBuffer() const
{
    return m_buffer;
}

// Convert Helper
VkAccessFlags ToVkAccessFlags(BufferUsageFlags usage)
{
    VkAccessFlags vkFlags = 0x00000000; // 0x00000000

    if (usage & BufferUsageFlagBits::kMapRead)
    {
        vkFlags |= VK_ACCESS_HOST_READ_BIT;
    }
    if (usage & BufferUsageFlagBits::kMapWrite)
    {
        vkFlags |= VK_ACCESS_HOST_WRITE_BIT;
    }
    if (usage & BufferUsageFlagBits::kCopySrc)
    {
        vkFlags |= VK_ACCESS_TRANSFER_READ_BIT;
    }
    if (usage & BufferUsageFlagBits::kCopyDst)
    {
        vkFlags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    }
    if (usage & BufferUsageFlagBits::kIndex)
    {
        vkFlags |= VK_ACCESS_INDEX_READ_BIT;
    }
    if (usage & BufferUsageFlagBits::kVertex)
    {
        vkFlags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    }
    if (usage & BufferUsageFlagBits::kUniform)
    {
        vkFlags |= VK_ACCESS_UNIFORM_READ_BIT;
    }

    return vkFlags;
}

VkPipelineStageFlags ToVkPipelineStageFlags(BufferUsageFlags usage)
{
    VkPipelineStageFlags flags = 0;

    if (usage & (BufferUsageFlagBits::kMapRead | BufferUsageFlagBits::kMapWrite))
    {
        flags |= VK_PIPELINE_STAGE_HOST_BIT;
    }
    if (usage & (BufferUsageFlagBits::kCopySrc | BufferUsageFlagBits::kCopyDst))
    {
        flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    if (usage & (BufferUsageFlagBits::kIndex | BufferUsageFlagBits::kVertex))
    {
        flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    }
    if (usage & (BufferUsageFlagBits::kUniform | BufferUsageFlagBits::kStorage))
    {
        // TODO: set by shader stage.
        flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }

    return flags;
}

VkBufferUsageFlags ToVkBufferUsageFlags(BufferUsageFlags usages)
{
    VkBufferUsageFlags vkUsages = 0x00000000;

    if (usages & BufferUsageFlagBits::kCopySrc)
    {
        vkUsages |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
    if (usages & BufferUsageFlagBits::kCopyDst)
    {
        vkUsages |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    if (usages & BufferUsageFlagBits::kIndex)
    {
        vkUsages |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
    if (usages & BufferUsageFlagBits::kVertex)
    {
        vkUsages |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if (usages & BufferUsageFlagBits::kUniform)
    {
        vkUsages |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    if (usages & BufferUsageFlagBits::kStorage)
    {
        vkUsages |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    return vkUsages;
}

// BufferUsageFlags ToBufferUsageFlags(VkAccessFlags vkflags)
// {
//     BufferUsageFlags flags = BufferUsageFlagBits::kInvalid; // 0x00000000

//     if (vkflags & VK_ACCESS_HOST_READ_BIT)
//     {
//         flags |= BufferUsageFlagBits::kMapRead;
//     }
//     if (vkflags & VK_ACCESS_HOST_WRITE_BIT)
//     {
//         flags |= BufferUsageFlagBits::kMapWrite;
//     }
//     if (vkflags & VK_ACCESS_TRANSFER_READ_BIT)
//     {
//         flags |= BufferUsageFlagBits::kCopySrc;
//     }
//     if (vkflags & VK_ACCESS_TRANSFER_WRITE_BIT)
//     {
//         flags |= BufferUsageFlagBits::kCopyDst;
//     }
//     if (vkflags & VK_ACCESS_INDEX_READ_BIT)
//     {
//         flags = BufferUsageFlagBits::kIndex | BufferUsageFlagBits::kCopyDst;
//     }
//     if (vkflags & VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)
//     {
//         flags = flags | BufferUsageFlagBits::kVertex;
//     }
//     if (vkflags & VK_ACCESS_UNIFORM_READ_BIT)
//     {
//         flags |= BufferUsageFlagBits::kUniform;
//     }

//     return flags;
// }

// BufferUsageFlags ToBufferUsageFlags(VkBufferUsageFlags vkUsages)
// {
//     BufferUsageFlags usages = BufferUsageFlagBits::kInvalid;

//     if (vkUsages & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
//     {
//         usages |= BufferUsageFlagBits::kCopySrc;
//     }
//     if (vkUsages & VK_BUFFER_USAGE_TRANSFER_DST_BIT)
//     {
//         usages |= BufferUsageFlagBits::kCopyDst;
//     }
//     if (vkUsages & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
//     {
//         usages |= BufferUsageFlagBits::kIndex;
//     }
//     if (vkUsages & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
//     {
//         usages |= BufferUsageFlagBits::kVertex;
//     }
//     if (vkUsages & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
//     {
//         usages |= BufferUsageFlagBits::kUniform;
//     }

//     return usages;
// }
} // namespace jipu
