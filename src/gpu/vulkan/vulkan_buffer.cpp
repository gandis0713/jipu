#include "vulkan_buffer.h"
#include "vulkan_device.h"
#include "vulkan_physical_device.h"

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

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;

    // find memory type. TODO: change location.
    {
        int memoryTypeIndex = -1;
        VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        auto vulkanPhysicalDevice = downcast(device->getPhysicalDevice());
        const auto& physicalDeviceInfo = vulkanPhysicalDevice->getInfo();
        for (int i = 0u; i < physicalDeviceInfo.memoryTypes.size(); ++i)
        {
            const auto& memoryType = physicalDeviceInfo.memoryTypes[i];
            if ((memoryType.propertyFlags & flags) == flags)
            {
                memoryTypeIndex = i;
                break;
            }
        }

        if (memoryTypeIndex == -1)
        {
            // TODO: delete VkBuffer resource automatically.
            device->vkAPI.DestroyBuffer(device->getVkDevice(), m_buffer, nullptr);
            throw std::runtime_error("Failed to find memory type index");
        }

        memoryAllocateInfo.memoryTypeIndex = static_cast<uint32_t>(memoryTypeIndex);
    }

    result = vkAPI.AllocateMemory(device->getVkDevice(), &memoryAllocateInfo, nullptr, &m_deviceMemory);
    if (result != VK_SUCCESS)
    {
        // TODO: delete VkBuffer resource automatically.
        device->vkAPI.DestroyBuffer(device->getVkDevice(), m_buffer, nullptr);
        throw std::runtime_error("Failed to allocate memory");
    }
}

VulkanBuffer::~VulkanBuffer()
{
    auto vulkanDevice = downcast(m_device);
    vulkanDevice->vkAPI.DestroyBuffer(vulkanDevice->getVkDevice(), m_buffer, nullptr);
}

VkBuffer VulkanBuffer::getVkBuffer() const
{
    return m_buffer;
}

} // namespace vkt